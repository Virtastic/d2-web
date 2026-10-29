// WebGL2 presenter (player page only). The 2D game canvas stays the drawing surface -- every blend,
// readback, harness read and screenshot still reads it -- and a GL canvas laid over it shows each
// finished frame through one GPU pass: upload the frame, look each channel up in the display ramp's
// 256-entry table with texelFetch. No filtering and no colour conversion, so the bytes on screen are
// the CPU pass's bytes (checked on all 2^24 colours against the table). The 2D canvas goes to opacity
// 0 on the first frame, so it keeps its hit testing, focus and cursor; the page's `canvas` rule
// (100% x 100% of #viewport, pixelated) sizes the layer exactly like it. A lost context removes the
// layer for good and the caller's Canvas2D path takes over again.
const VS = `#version 300 es
void main() { gl_Position = vec4(gl_VertexID == 1 ? 3.0 : -1.0, gl_VertexID == 2 ? 3.0 : -1.0, 0.0, 1.0); }`;
const FS = `#version 300 es
precision highp float;
uniform highp sampler2D u_src, u_lut;
uniform int u_h, u_ramp;
out vec4 o;
float lut(float c) { return texelFetch(u_lut, ivec2(int(c * 255.0 + 0.5), 0), 0).r; }
void main() {
  vec4 c = texelFetch(u_src, ivec2(gl_FragCoord.x, float(u_h) - gl_FragCoord.y), 0);
  o = u_ramp != 0 ? vec4(lut(c.r), lut(c.g), lut(c.b), 1.0) : vec4(c.rgb, 1.0);
}`;

export function createGlPresenter(src, ramp) {
  const el = src.ownerDocument.createElement('canvas');
  const gl = el.getContext('webgl2', { alpha: false, antialias: false, depth: false, stencil: false });
  if (!gl) return null;
  const prog = gl.createProgram();
  for (const [type, text] of [[gl.VERTEX_SHADER, VS], [gl.FRAGMENT_SHADER, FS]]) {
    const s = gl.createShader(type); gl.shaderSource(s, text); gl.compileShader(s); gl.attachShader(prog, s);
  }
  gl.linkProgram(prog);
  if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) return null;
  gl.useProgram(prog);
  const tex = (unit) => {
    const t = gl.createTexture(); gl.activeTexture(gl.TEXTURE0 + unit); gl.bindTexture(gl.TEXTURE_2D, t);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST); gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    return t;
  };
  tex(1); gl.texImage2D(gl.TEXTURE_2D, 0, gl.R8, 256, 1, 0, gl.RED, gl.UNSIGNED_BYTE, Uint8Array.from(ramp));
  let frame = tex(0);   // unit 0 stays active from here on
  gl.uniform1i(gl.getUniformLocation(prog, 'u_src'), 0); gl.uniform1i(gl.getUniformLocation(prog, 'u_lut'), 1);
  const uH = gl.getUniformLocation(prog, 'u_h'), uRamp = gl.getUniformLocation(prog, 'u_ramp');
  // The game canvas is opaque, so premultiplied == straight: uploading it premultiplied skips the
  // browser's unpremultiply pass (1080p upload 10.3 -> 4.5 ms in Chrome), same bytes.
  gl.pixelStorei(gl.UNPACK_COLORSPACE_CONVERSION_WEBGL, gl.NONE); gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, true);
  gl.bindVertexArray(gl.createVertexArray());
  // Hidden until the first present, so its empty buffer never covers a frame the 2D canvas already shows.
  Object.assign(el.style, { position: 'absolute', left: '0', top: '0', pointerEvents: 'none', visibility: 'hidden' });
  src.after(el);
  let w = 0, h = 0, lost = false;
  el.addEventListener('webglcontextlost', () => { lost = true; el.remove(); src.style.opacity = ''; });
  return {
    get ok() { return !lost; },
    gl, el,
    /** Show what the 2D canvas holds now; `rampOn` applies the display ramp (the canvas is unramped). */
    present(rampOn) {
      if (lost) return false;
      if (src.width !== w || src.height !== h) {
        w = el.width = src.width; h = el.height = src.height;
        gl.deleteTexture(frame); frame = tex(0); gl.texStorage2D(gl.TEXTURE_2D, 1, gl.RGBA8, w, h);
        gl.viewport(0, 0, w, h); gl.uniform1i(uH, h);
      }
      gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, gl.RGBA, gl.UNSIGNED_BYTE, src);
      gl.uniform1i(uRamp, rampOn ? 1 : 0);
      gl.drawArrays(gl.TRIANGLES, 0, 3);
      if (src.style.opacity !== '0') { el.style.visibility = ''; src.style.opacity = '0'; }
      return true;
    },
  };
}
