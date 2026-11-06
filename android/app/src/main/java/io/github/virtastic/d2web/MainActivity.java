package io.github.virtastic.d2web;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.webkit.ConsoleMessage;
import android.webkit.PermissionRequest;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

/**
 * A full-screen, landscape WebView onto the packaged site (SiteHandler). The page is the same index.html the
 * browser build serves, with the same engine; it runs in bring-your-own-files mode: its front door asks for the
 * player's Diablo II archives (the system file picker below) and keeps them in the page's own storage.
 *
 * The engine needs WebAssembly Memory64 and a crossOriginIsolated page. Both come from Android System WebView;
 * when the installed one has neither, the app says so and offers the web version in the browser.
 */
public class MainActivity extends Activity {
    static final String TAG = "d2-web";
    static final String WEB_VERSION = "https://diablo2.virtastic.app/";
    private static final int PICK = 1;
    // A module that declares one 64-bit memory: valid only where Memory64 ships.
    private static final String PROBE = "(() => { let m64 = false; try { m64 = WebAssembly.validate(new Uint8Array("
            + "[0,97,115,109,1,0,0,0,5,3,1,4,1])); } catch (e) {} return JSON.stringify({ isolated: self.crossOriginIsolated === true,"
            + " sab: typeof SharedArrayBuffer !== 'undefined', memory64: m64, opfs: !!(navigator.storage && navigator.storage.getDirectory),"
            + " secure: self.isSecureContext, href: location.href, ua: navigator.userAgent }); })()";

    private WebView web;
    private ValueCallback<Uri[]> pending;
    private boolean probed;

    @Override
    protected void onCreate(Bundle saved) {
        super.onCreate(saved);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if ((getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0) WebView.setWebContentsDebuggingEnabled(true);

        web = new WebView(this);
        web.setBackgroundColor(0xff000000);
        setContentView(web);
        immersive();

        WebSettings s = web.getSettings();
        s.setJavaScriptEnabled(true);
        s.setDomStorageEnabled(true);
        s.setDatabaseEnabled(true);
        s.setMediaPlaybackRequiresUserGesture(false);
        s.setAllowFileAccess(false);
        s.setAllowContentAccess(false);
        s.setMixedContentMode(WebSettings.MIXED_CONTENT_NEVER_ALLOW);
        s.setSupportZoom(false);
        s.setBuiltInZoomControls(false);
        s.setUseWideViewPort(true);
        s.setLoadWithOverviewMode(true);
        s.setTextZoom(100);
        s.setSupportMultipleWindows(false);

        final SiteHandler site = new SiteHandler(getAssets());
        web.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest req) {
                return site.handle(req);
            }

            @Override
            public boolean shouldOverrideUrlLoading(WebView view, WebResourceRequest req) {
                if (SiteHandler.isOurs(req.getUrl())) return false;
                openExternal(req.getUrl());   // links leave the app for the browser
                return true;
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                if (probed) return;
                probed = true;
                view.evaluateJavascript(PROBE, MainActivity.this::onProbe);
            }
        });
        web.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onShowFileChooser(WebView view, ValueCallback<Uri[]> callback, FileChooserParams params) {
                if (pending != null) pending.onReceiveValue(null);
                pending = callback;
                Intent i = new Intent(Intent.ACTION_OPEN_DOCUMENT);
                i.addCategory(Intent.CATEGORY_OPENABLE);
                i.setType("*/*");   // .mpq has no MIME type of its own; the page checks names and contents
                i.putExtra(Intent.EXTRA_ALLOW_MULTIPLE, params.getMode() != FileChooserParams.MODE_OPEN);
                try {
                    startActivityForResult(i, PICK);
                } catch (ActivityNotFoundException e) {
                    pending = null;
                    callback.onReceiveValue(null);
                }
                return true;
            }

            @Override
            public void onPermissionRequest(PermissionRequest request) { request.deny(); }

            @Override
            public boolean onConsoleMessage(ConsoleMessage m) {
                if (m.messageLevel() == ConsoleMessage.MessageLevel.ERROR || m.messageLevel() == ConsoleMessage.MessageLevel.WARNING)
                    Log.w(TAG, "console: " + m.message() + " @" + m.sourceId() + ":" + m.lineNumber());
                return true;
            }
        });

        if (saved != null) web.restoreState(saved);
        else web.loadUrl(SiteHandler.ORIGIN);
    }

    private void onProbe(String json) {
        Log.i(TAG, "probe " + json);
        if (json == null) return;
        String j = json.replace("\\\"", "\"");
        if (j.contains("\"isolated\":true") && j.contains("\"memory64\":true")) return;
        new AlertDialog.Builder(this)
                .setTitle(R.string.unsupported_title)
                .setMessage(R.string.unsupported_body)
                .setPositiveButton(R.string.open_in_chrome, (d, w) -> openExternal(Uri.parse(WEB_VERSION)))
                .setNegativeButton(R.string.close, null)
                .show();
    }

    private void openExternal(Uri u) {
        try { startActivity(new Intent(Intent.ACTION_VIEW, u)); } catch (ActivityNotFoundException e) { Log.w(TAG, "no browser for " + u); }
    }

    @Override
    protected void onActivityResult(int request, int result, Intent data) {
        super.onActivityResult(request, result, data);
        if (request != PICK || pending == null) return;
        Uri[] uris = null;
        if (result == RESULT_OK && data != null) {
            if (data.getClipData() != null) {
                uris = new Uri[data.getClipData().getItemCount()];
                for (int k = 0; k < uris.length; k++) uris[k] = data.getClipData().getItemAt(k).getUri();
            } else if (data.getData() != null) {
                uris = new Uri[] { data.getData() };
            }
        }
        pending.onReceiveValue(uris);
        pending = null;
    }

    /** The system back button is the game's Esc (the in-game menu); leaving the app is the home gesture. */
    @Override
    public boolean onKeyDown(int code, KeyEvent e) {
        if (code == KeyEvent.KEYCODE_BACK) {
            web.dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_ESCAPE));
            return true;
        }
        return super.onKeyDown(code, e);
    }

    @Override
    public boolean onKeyUp(int code, KeyEvent e) {
        if (code == KeyEvent.KEYCODE_BACK) {
            web.dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_ESCAPE));
            return true;
        }
        return super.onKeyUp(code, e);
    }

    @SuppressWarnings("deprecation")
    private void immersive() {
        if (Build.VERSION.SDK_INT >= 30) {
            getWindow().setDecorFitsSystemWindows(false);
            WindowInsetsController c = getWindow().getInsetsController();
            if (c != null) {
                c.hide(WindowInsets.Type.systemBars());
                c.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        }
    }

    @Override
    public void onWindowFocusChanged(boolean focus) {
        super.onWindowFocusChanged(focus);
        if (focus) immersive();
    }

    @Override
    protected void onSaveInstanceState(Bundle out) {
        super.onSaveInstanceState(out);
        web.saveState(out);
    }

    @Override
    protected void onPause() { super.onPause(); web.onPause(); }

    @Override
    protected void onResume() { super.onResume(); web.onResume(); }

    @Override
    protected void onDestroy() {
        if (web != null) web.destroy();
        super.onDestroy();
    }
}
