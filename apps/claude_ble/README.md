# Claude BLE

A Flipper Zero app that wirelessly bridges to Claude AI via a custom BLE GATT service.

## Architecture

```
[Flipper Zero] ──WRITE──► [Companion App] ──HTTPS──► [Claude API]
               ◄──NOTIFY─                 ◄──JSON───
```

**Flipper side (this app):**
- Advertises a custom BLE GATT service
- Text input UI for typing a prompt (up to 256 chars)
- WRITEs the prompt to the Prompt characteristic
- Shows a waiting screen until a NOTIFY arrives on the Response characteristic
- Displays the response in a scrollable text box

**Companion side (separate project in `faulty/`):**
- Scans for and connects to the Flipper's GATT service
- Subscribes to Response characteristic notifications
- On Prompt WRITE → calls Claude API (`claude-sonnet-4-6`)
- Chunks the response and NOTIFYs it back over BLE

## GATT Service Definition

| Role | UUID | Properties |
|------|------|------------|
| Service | `C1A00001-0000-1000-8000-00805F9B34FB` | — |
| Prompt | `C1A00002-0000-1000-8000-00805F9B34FB` | WRITE\_WITHOUT\_RESP |
| Response | `C1A00003-0000-1000-8000-00805F9B34FB` | NOTIFY |

Max payload per packet: 244 bytes (MTU 247 − 3-byte ATT header).
Longer responses are chunked by the companion and reassembled on the Flipper.

## Build

```sh
# Install ufbt once
pip install ufbt

# Build the FAP
cd apps/claude_ble
ufbt

# Build + deploy to connected Flipper
ufbt launch
```

## BLE Implementation Note

`claude_ble_gatt.c` contains two implementation paths with `TODO` markers:

**Path A — Momentum / RogueMaster firmware:**  
Uses `aci_gatt_add_service()` / `aci_gatt_add_char()` from the ST BLE stack to
register a true custom GATT service with the correct UUIDs above.

**Path B — Stock OFW:**  
Repurposes the built-in BLE serial profile characteristics as the data channel
(`furi_hal_ble_serial_tx()` / serial event callback). The UUIDs will differ but
the rest of the app (UI, scenes, protocol) is identical.
