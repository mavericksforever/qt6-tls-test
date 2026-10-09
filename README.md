# Qt TLS Test

Minimal Qt 6 app that makes one HTTPS GET request on launch and shows what Qt's TLS layer reports. Built against the [Qt 6.8.3 Mavericks port](https://github.com/mavericksforever/qtbase) for OS X 10.9.

## Expected results on OS X 10.9

| Setup | Result |
|---|---|
| AquaTransport installed, default settings | `FAILED: SSL handshake failed: Protocol version mismatch` |
| AquaTransport installed, "Accept any TLS version" checked | `OK: HTTP …`, negotiated protocol reported to Qt as TLS 1.0 |

The second row shows the connection itself works and only Qt's own version check rejects it.

A URL can be passed as the first argument:

```
QtTlsTest.app/Contents/MacOS/QtTlsTest https://example.com/
```

## Building

Push to GitHub and run the `Build Qt TLS Test for macOS 10.9` workflow. The DMG is uploaded as a build artifact.
