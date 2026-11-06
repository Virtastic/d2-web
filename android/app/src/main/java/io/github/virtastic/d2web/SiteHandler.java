package io.github.virtastic.d2web;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.net.Uri;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Answers the page's requests from the site packaged in assets/site, the way the desktop app's app:// handler
 * and the browser build's server do: COOP/COEP/CORP on every answer (so the page is crossOriginIsolated and
 * gets SharedArrayBuffer), HTTP Range (206) and HEAD. The origin is https://appassets.androidplatform.net,
 * the host Android reserves for app-packaged content, so the page is a secure context.
 *
 * No game data is packaged: the page asks the player for their own files and keeps them in its own storage.
 */
final class SiteHandler {
    static final String HOST = "appassets.androidplatform.net";
    static final String ORIGIN = "https://" + HOST + "/";
    private static final String ROOT = "site/";
    private static final Pattern RANGE = Pattern.compile("^bytes=(\\d*)-(\\d*)$");
    private static final Map<String, String> TYPES = new HashMap<>();
    static {
        TYPES.put("html", "text/html");
        TYPES.put("js", "text/javascript");
        TYPES.put("mjs", "text/javascript");
        TYPES.put("cjs", "text/javascript");
        TYPES.put("wasm", "application/wasm");
        TYPES.put("json", "application/json");
        TYPES.put("webmanifest", "application/manifest+json");
        TYPES.put("svg", "image/svg+xml");
        TYPES.put("png", "image/png");
        TYPES.put("jpg", "image/jpeg");
        TYPES.put("css", "text/css");
        TYPES.put("txt", "text/plain");
        TYPES.put("md", "text/markdown");
    }

    private final AssetManager assets;

    SiteHandler(AssetManager assets) { this.assets = assets; }

    static boolean isOurs(Uri u) {
        return u != null && "https".equals(u.getScheme()) && HOST.equals(u.getHost());
    }

    /** null for a request that is not ours (the WebView handles it). */
    WebResourceResponse handle(WebResourceRequest req) {
        Uri u = req.getUrl();
        if (!isOurs(u)) return null;
        String method = req.getMethod() == null ? "GET" : req.getMethod().toUpperCase(Locale.ROOT);
        if (!method.equals("GET") && !method.equals("HEAD")) return error(405, "Method Not Allowed");
        String path = u.getPath() == null ? "/" : u.getPath();
        String rel = path.replaceAll("^/+", "");
        if (rel.isEmpty() || rel.endsWith("/")) rel += "index.html";
        for (String seg : rel.split("/")) {
            if (seg.isEmpty() || seg.equals(".") || seg.equals("..")) return error(404, "Not Found");
        }
        if (rel.contains("\\") || rel.indexOf('\0') >= 0) return error(404, "Not Found");

        String name = rel.substring(rel.lastIndexOf('/') + 1);
        String ext = name.contains(".") ? name.substring(name.lastIndexOf('.') + 1).toLowerCase(Locale.ROOT) : "";
        String type = TYPES.containsKey(ext) ? TYPES.get(ext) : "application/octet-stream";
        boolean head = method.equals("HEAD");

        try {
            long size;
            InputStream in;
            AssetFileDescriptor fd = null;
            try { fd = assets.openFd(ROOT + rel); } catch (IOException compressed) { /* stored compressed: read below */ }
            if (fd != null) {
                size = fd.getLength();
                in = fd.createInputStream();
            } else {
                byte[] all = readAll(assets.open(ROOT + rel, AssetManager.ACCESS_STREAMING));
                size = all.length;
                in = new ByteArrayInputStream(all);
            }

            Map<String, String> h = security();
            h.put("Accept-Ranges", "bytes");
            h.put("Cache-Control", rel.startsWith("e/") ? "public, max-age=31536000, immutable" : "no-cache");

            long start = 0, end = size - 1;
            int status = 200;
            String reason = "OK";
            String range = header(req, "Range");
            if (range != null) {
                Matcher m = RANGE.matcher(range.trim());
                if (m.matches() && !(m.group(1).isEmpty() && m.group(2).isEmpty())) {
                    if (m.group(1).isEmpty()) start = Math.max(0, size - Long.parseLong(m.group(2)));
                    else start = Long.parseLong(m.group(1));
                    if (!m.group(1).isEmpty() && !m.group(2).isEmpty()) end = Math.min(Long.parseLong(m.group(2)), size - 1);
                    if (start > end || start >= size) {
                        in.close();
                        Map<String, String> bad = security();
                        bad.put("Content-Range", "bytes */" + size);
                        return new WebResourceResponse(type, null, 416, "Range Not Satisfiable", bad, empty());
                    }
                    status = 206;
                    reason = "Partial Content";
                    h.put("Content-Range", "bytes " + start + "-" + end + "/" + size);
                }
            }
            long len = end - start + 1;
            h.put("Content-Length", Long.toString(len));
            if (head) { in.close(); return new WebResourceResponse(type, null, status, reason, h, empty()); }
            skipFully(in, start);
            String enc = type.startsWith("text/") || type.endsWith("json") || type.endsWith("javascript") ? "utf-8" : null;
            return new WebResourceResponse(type, enc, status, reason, h, new Bounded(in, len));
        } catch (FileNotFoundException e) {
            return error(404, "Not Found");
        } catch (IOException e) {
            return error(500, "Internal Server Error");
        }
    }

    private static String header(WebResourceRequest req, String name) {
        Map<String, String> hs = req.getRequestHeaders();
        if (hs == null) return null;
        for (Map.Entry<String, String> e : hs.entrySet()) if (name.equalsIgnoreCase(e.getKey())) return e.getValue();
        return null;
    }

    private static Map<String, String> security() {
        Map<String, String> h = new HashMap<>();
        h.put("Cross-Origin-Opener-Policy", "same-origin");
        h.put("Cross-Origin-Embedder-Policy", "require-corp");
        h.put("Cross-Origin-Resource-Policy", "cross-origin");
        return h;
    }

    private static WebResourceResponse error(int status, String reason) {
        Map<String, String> h = security();
        h.put("Cache-Control", "no-store");
        return new WebResourceResponse("text/plain", "utf-8", status, reason, h,
                new ByteArrayInputStream(reason.getBytes(StandardCharsets.UTF_8)));
    }

    private static InputStream empty() { return new ByteArrayInputStream(new byte[0]); }

    private static byte[] readAll(InputStream in) throws IOException {
        try (InputStream s = in) {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buf = new byte[65536];
            for (int n; (n = s.read(buf)) > 0; ) out.write(buf, 0, n);
            return out.toByteArray();
        }
    }

    private static void skipFully(InputStream in, long n) throws IOException {
        while (n > 0) {
            long k = in.skip(n);
            if (k <= 0) { if (in.read() < 0) throw new IOException("short asset"); k = 1; }
            n -= k;
        }
    }

    /** The response body: at most len bytes of the asset, then end of stream. */
    private static final class Bounded extends InputStream {
        private final InputStream in;
        private long left;
        Bounded(InputStream in, long left) { this.in = in; this.left = left; }
        @Override public int read() throws IOException {
            if (left <= 0) return -1;
            int b = in.read();
            if (b >= 0) left--;
            return b;
        }
        @Override public int read(byte[] b, int off, int len) throws IOException {
            if (left <= 0) return -1;
            int n = in.read(b, off, (int) Math.min(len, left));
            if (n > 0) left -= n;
            return n;
        }
        @Override public void close() throws IOException { in.close(); }
    }
}
