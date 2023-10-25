# mac-power-monitor

[![Test Service](https://github.com/oomol-lab/mac-power-monitor/actions/workflows/test-service.yml/badge.svg)](https://github.com/oomol-lab/mac-power-monitor/actions/workflows/test-service.yml)

```typescript
import { createMacPowerMonitor } from "@oomol-lab/mac-power-monitor";

const monitor = createMacPowerMonitor();

monitor.listenOnWillSleep(function () {
    console.log("will sleep");
});

monitor.listenOnWillWake(function () {
    console.log("will wake");
});

// when we don't need it
monitor.dispose();
```