// Browser equivalent of D2Win's Print Screen command, including its numbered JPEG output.
export function createScreenshotAction({ documentObject = globalThis.document,
  urlObject = globalThis.URL, now = () => Date.now() } = {}) {
  let lastCaptureAt = 0;
  let screenshotIndex = 0;

  return (canvas) => {
    if (!canvas || typeof canvas.toBlob !== 'function') return { captured: false, reason: 'canvas-unavailable' };
    const at = Number(now());
    if (!Number.isFinite(at)) return { captured: false, reason: 'clock-unavailable' };
    if (at - lastCaptureAt < 1000) return { captured: false, reason: 'rate-limited' };

    const filename = `Screenshot${String(screenshotIndex++).padStart(3, '0')}.jpg`;
    lastCaptureAt = at;
    canvas.toBlob((blob) => {
      if (!blob || !documentObject?.createElement || typeof urlObject?.createObjectURL !== 'function') return;
      const href = urlObject.createObjectURL(blob);
      const link = documentObject.createElement('a');
      link.href = href;
      link.download = filename;
      link.click();
      setTimeout(() => urlObject.revokeObjectURL?.(href), 0);
    }, 'image/jpeg', 1);
    return { captured: true, filename };
  };
}
