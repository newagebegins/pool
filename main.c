#include <windows.h>
#include <stdbool.h>
#include <math.h>

#define PI 3.14159265358979323846f

#define ARR_LEN(arr) (sizeof(arr)/sizeof(*arr))

typedef struct {
  float x;
  float y;
} Vec2;

Vec2 makeVec2(float x, float y) {
  Vec2 v;
  v.x = x;
  v.y = y;
  return v;
}

Vec2 addVec2(Vec2 v1, Vec2 v2) {
  Vec2 v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  return v;
}

Vec2 scaleVec2(Vec2 v, float s) {
  Vec2 r;
  r.x = v.x * s;
  r.y = v.y * s;
  return r;
}

typedef struct {
  float e[4];
} Mat2;

Mat2 makeScalingMat2(float scale) {
  Mat2 m;
  m.e[0] = m.e[3] = scale;
  m.e[1] = m.e[2] = 0;
  return m;
}

Mat2 makeRotationMat2(float angle) {
  Mat2 m;
  m.e[0] = cosf(angle);
  m.e[1] = -sinf(angle);
  m.e[2] = sinf(angle);
  m.e[3] = cosf(angle);
  return m;
}

Mat2 mulMat2(Mat2 m1, Mat2 m2) {
  Mat2 m;
  m.e[0] = m1.e[0]*m2.e[0] + m1.e[1]*m2.e[2];
  m.e[1] = m1.e[0]*m2.e[1] + m1.e[1]*m2.e[3];
  m.e[2] = m1.e[2]*m2.e[0] + m1.e[3]*m2.e[2];
  m.e[3] = m1.e[2]*m2.e[1] + m1.e[3]*m2.e[3];
  return m;
}

Vec2 mulMatVec2(Mat2 m, Vec2 v) {
  Vec2 r;
  r.x = m.e[0]*v.x + m.e[1]*v.y;
  r.y = m.e[2]*v.x + m.e[3]*v.y;
  return r;
}

typedef enum {
  COLOR_BLACK   = 0xFF000000,
  COLOR_WHITE   = 0xFFFFFFFF,
  COLOR_GREEN   = 0xFF00FF00,
  COLOR_RED     = 0xFFFF0000,
  COLOR_BLUE    = 0xFF0000FF,
  COLOR_YELLOW  = 0xFFFFFF00,
  COLOR_MAGENTA = 0xFFFF00FF,
  COLOR_CYAN    = 0xFF00FFFF,
  COLOR_PINK    = 0xFFF6A5D1,
} Color;

typedef struct {
  bool leftFlipperDown;
  bool rightFlipperDown;
} Buttons;

typedef struct {
  Color *memory;
  size_t size;
  int width;
  int height;
  int windowWidth;
  int windowHeight;
  HDC deviceContext;
  BITMAPINFO info;
} BackBuffer;

BackBuffer makeBackBuffer(int width, int height) {
  BackBuffer bb = {0};

  bb.size = width * height * sizeof(*bb.memory);
  bb.memory = malloc(bb.size);

  bb.width = width;
  bb.height = height;

  bb.info.bmiHeader.biSize = sizeof(bb.info.bmiHeader);
  bb.info.bmiHeader.biWidth = width;
  bb.info.bmiHeader.biHeight = height;
  bb.info.bmiHeader.biPlanes = 1;
  bb.info.bmiHeader.biBitCount = 32;
  bb.info.bmiHeader.biCompression = BI_RGB;

  return bb;
}

void clearBackBuffer(BackBuffer *bb, Color color) {
  memset(bb->memory, color, bb->size);
}

void setPixel(BackBuffer *bb, int x, int y, Color color) {
  bb->memory[y*bb->width + x] = color;
}

void drawCircle(BackBuffer *bb, Vec2 position, float radiusF, Color color) {
  int x0 = (int)position.x;
  int y0 = (int)position.y;
  int radius = (int)radiusF;
  
  int x = radius;
  int y = 0;
  int radiusSquared = radius*radius;

  while (x >= y) {
    setPixel(bb, x0 + x, y0 + y, color);
    setPixel(bb, x0 + y, y0 + x, color);
    setPixel(bb, x0 - y, y0 + x, color);
    setPixel(bb, x0 - x, y0 + y, color);
    setPixel(bb, x0 - x, y0 - y, color);
    setPixel(bb, x0 - y, y0 - x, color);
    setPixel(bb, x0 + y, y0 - x, color);
    setPixel(bb, x0 + x, y0 - y, color);

    y += 1;

    int ySquared = y*y;
    int oldXError = abs(x*x + ySquared - radiusSquared);
    int newXError = abs((x-1)*(x-1) + ySquared - radiusSquared);

    if (newXError < oldXError) {
      x -= 1;
    }
  }
}

void drawLine(BackBuffer *bb, Vec2 v1, Vec2 v2, Color color) {
  int x1 = (int)v1.x;
  int y1 = (int)v1.y;
  int x2 = (int)v2.x;
  int y2 = (int)v2.y;

  if (x1 == x2 && y1 == y2) {
    setPixel(bb, x1, y1, color);
    return;
  }

  int xStart, xEnd, yStart, yEnd;
  int dx = x2 - x1;
  int dy = y2 - y1;

  if (abs(dx) > abs(dy)) {
    float m = (float)dy / (float)dx;
    if (x1 < x2) {
      xStart = x1;
      yStart = y1;
      xEnd = x2;
      yEnd = y2;
    } else {
      xStart = x2;
      yStart = y2;
      xEnd = x1;
      yEnd = y1;
    }
    for (int x = xStart; x <= xEnd; ++x) {
      int y = (int)(m * (x - xStart) + yStart);
      setPixel(bb, x, y, color);
    }
  } else {
    float m = (float)dx / (float)dy;
    if (y1 < y2) {
      xStart = x1;
      yStart = y1;
      xEnd = x2;
      yEnd = y2;
    } else {
      xStart = x2;
      yStart = y2;
      xEnd = x1;
      yEnd = y1;
    }
    for (int y = yStart; y <= yEnd; ++y) {
      int x = (int)(m * (y - yStart) + xStart);
      setPixel(bb, x, y, color);
    }
  }
}

typedef struct {
  Vec2 position;
  Vec2 velocity;
  Vec2 acceleration;
  float radius;
  Color color;
} Ball;

LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(wnd, msg, wparam, lparam);
  }
  return 0;
}

int CALLBACK WinMain(HINSTANCE inst, HINSTANCE prevInst, LPSTR cmdLine, int cmdShow) {
  UNREFERENCED_PARAMETER(prevInst);
  UNREFERENCED_PARAMETER(cmdLine);

  WNDCLASS wndClass = {0};
  wndClass.style = CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = wndProc;
  wndClass.hInstance = inst;
  wndClass.hCursor = LoadCursor(0, IDC_ARROW);
  wndClass.lpszClassName = "Pool";
  RegisterClass(&wndClass);

  int windowWidth = 1920/2;
  int windowHeight = 1080/2;

  RECT crect = {0};
  crect.right = windowWidth;
  crect.bottom = windowHeight;

  DWORD wndStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
  AdjustWindowRect(&crect, wndStyle, 0);

  HWND wnd = CreateWindowEx(0, wndClass.lpszClassName, "Pool", wndStyle, 300, 0,
                            crect.right - crect.left, crect.bottom - crect.top,
                            0, 0, inst, 0);
  ShowWindow(wnd, cmdShow);
  UpdateWindow(wnd);

  float dt = 0.0f;
  LARGE_INTEGER perfcFreq = {0};
  LARGE_INTEGER perfc = {0};
  LARGE_INTEGER prefcPrev = {0};

  QueryPerformanceFrequency(&perfcFreq);
  QueryPerformanceCounter(&perfc);

  bool running = true;
  HDC deviceContext = GetDC(wnd);
  BackBuffer bb = makeBackBuffer(windowWidth/4, windowHeight/4);
  Buttons buttons = {0};

  Ball balls[2] = {0};
  {
    int i = 0;
    float radius = 10.0f;

    balls[i].position.x = (float) bb.width / 2;
    balls[i].position.y = (float) bb.height / 2;
    balls[i].radius = radius;
    balls[i].color = COLOR_RED;

    ++i;

    balls[i].position.x = (float) bb.width / 4;
    balls[i].position.y = (float) bb.height / 2;
    balls[i].radius = radius;
    balls[i].color = COLOR_GREEN;
  }

  while (running) {
    prefcPrev = perfc;
    QueryPerformanceCounter(&perfc);
    dt = (float)(perfc.QuadPart - prefcPrev.QuadPart) / (float)perfcFreq.QuadPart;

    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      switch (msg.message) {
        case WM_QUIT:
          running = false;
          break;

        case WM_KEYDOWN:
        case WM_KEYUP:
          bool isDown = ((msg.lParam & (1 << 31)) == 0);
          switch (msg.wParam) {
            case VK_ESCAPE:
              running = false;
              break;
            case 0x5A: // Z
              buttons.leftFlipperDown = isDown;
              break;
            case 0xBF: // slash
              buttons.rightFlipperDown = isDown;
              break;
          }
          break;

        default:
          TranslateMessage(&msg);
          DispatchMessage(&msg);
          break;
      }
    }

    clearBackBuffer(&bb, COLOR_BLACK);

    for (int i = 0; i < ARR_LEN(balls); ++i) {
      Ball b = balls[i];
      drawCircle(&bb, b.position, b.radius, b.color);
    }

    StretchDIBits(deviceContext, 0, 0, windowWidth, windowHeight,
                  0, 0, bb.width, bb.height, bb.memory,
                  &bb.info, DIB_RGB_COLORS, SRCCOPY);
  }
}