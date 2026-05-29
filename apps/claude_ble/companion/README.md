# Claude BLE Companion

React Native app that bridges the Flipper Zero's BLE GATT service to the Claude API.

## How it works

```
[Flipper] user types prompt
    │ NOTIFY on Prompt char (C1A00002)
    ▼
[Companion] receives prompt → calls Claude API (claude-sonnet-4-6)
    │ WRITE chunks to Response char (C1A00003)
    │ final \x00 byte = end-of-transmission sentinel
    ▼
[Flipper] accumulates chunks → displays response
```

## Setup

### 1. Init the React Native project

```sh
npx react-native@0.74 init ClaudeBleCompanion
cd ClaudeBleCompanion
```

Then copy the files from this directory into the initialized project and run:

```sh
npm install
```

### 2. Set your API key

```sh
cp .env.example .env
# open .env and fill in your Anthropic API key
```

### 3. Native permissions

**iOS** — add to `ios/<AppName>/Info.plist`:
```xml
<key>NSBluetoothAlwaysUsageDescription</key>
<string>Used to communicate with your Flipper Zero.</string>
```

**Android** — add to `android/app/src/main/AndroidManifest.xml`:
```xml
<uses-permission android:name="android.permission.BLUETOOTH_SCAN"
    android:usesPermissionFlags="neverForLocation" />
<uses-permission android:name="android.permission.BLUETOOTH_CONNECT" />
<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
```

### 4. Run

```sh
npm run android   # or: npm run ios
```

## Response length

`MAX_TOKENS` in `src/api/claude.ts` is set to 150 (~400–500 chars) to fit the
Flipper's 512-byte response buffer. To allow longer responses, increase both
`MAX_TOKENS` here and `CLAUDE_BLE_RESPONSE_MAX_LEN` in `../claude_ble_app.h`,
then update `on_response_written()` in `../claude_ble_gatt.c` to handle
multi-chunk reassembly properly.

## Security

Your Anthropic API key is bundled via `react-native-config`. Acceptable for
a personal paired-device tool. Never commit `.env` to version control —
`.gitignore` already covers it.
