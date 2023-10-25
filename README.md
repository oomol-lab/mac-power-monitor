# mac-power-monitor

[![Test Service](https://github.com/oomol-lab/mac-power-monitor/actions/workflows/test-service.yml/badge.svg)](https://github.com/oomol-lab/mac-power-monitor/actions/workflows/test-service.yml)

> Inspiration from: [prashantgupta24/mac-sleep-notifier](https://github.com/prashantgupta24/mac-sleep-notifier)

## Install

```shell
pnpm add @oomol-lab/mac-power-monitor
# or
npm install @oomol-lab/mac-power-monitor
# or
yarn add @oomol-lab/mac-power-monitor
```

## Usage

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