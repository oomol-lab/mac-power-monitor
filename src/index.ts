let current: MacPowerMonitor | null = null;

export type Disposer = () => void;

export interface MacPowerMonitor {
    canSleep: boolean;
    listenOnWillSleep(listener: () => void): Disposer;
    listenOnWillWake(listener: () => void): Disposer;
    dispose(): void;
    unregisterAll(): void;
}

export async function createMacPowerMonitor(): Promise<MacPowerMonitor> {
    if (current) {
        throw new Error("did create. should dispose the previous");
    }
    let didStop = false;

    const path = "./mac-power-monitor.node";
    const node = require(path);
    const monitor: Omit<MacPowerMonitor, "canSleep"> = {
        unregisterAll: () => {
            assertIsRunning();
            node.unregister();
        },
        listenOnWillSleep: (listener) => {
            assertIsRunning();
            const id = node.registerOnWillSleep(listener);
            return () => {
                node.unregisterOnWillSleep(id);
            };
        },
        listenOnWillWake: (listener) => {
            assertIsRunning();
            const id = node.registerOnWillWake(listener);
            return () => {
                node.unregisterOnWillWake(id);
            };
        },
        dispose: () => {
            assertIsRunning();
            didStop = true;
            current = null;
        },
    };
    Object.defineProperty(monitor, "canSleep", {
        configurable: false,
        set: (canSleep) => {
            assertIsRunning();
            node.setCanSleep(canSleep);
        },
        get: () => {
            assertIsRunning();
            return node.canSleep();
        },
    });
    function assertIsRunning(): void {
        if (didStop) {
            throw new Error("mac power monitor did call dispose()");
        }
    }
    node.start();
    current = monitor as MacPowerMonitor;

    return current;
}

function fileURLToPath(url: string) {
    throw new Error("Function not implemented.");
}
