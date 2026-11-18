// 1.13c D2sound+0x0B2D0 -- thunk: forwards ST1/ST0 doubles to the SSE2 math body at 0xB2E9 (host seam, unported).
double D2sound_B2D0(double st1, double st0, double (*body_B2E9)(double, double)) {
    return body_B2E9(st1, st0);
}
