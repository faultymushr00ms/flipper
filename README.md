# flipper

A collection of apps for the Flipper Zero.

## Structure

Each app lives in its own directory under `apps/`:

```
apps/
  <app_name>/
    application.fam   # app manifest (id, name, version, category)
    <app_name>.c      # entry point
    README.md         # what the app does and how to build/install
```

## Building

Apps are built with [ufbt](https://github.com/flipperdevices/flipperzero-ufbt).

```sh
# install ufbt once
pip install ufbt

# build a specific app
cd apps/<app_name>
ufbt

# build + launch on connected Flipper
ufbt launch
```

## Adding a new app

1. Create `apps/<your_app_name>/`
2. Add `application.fam` with a unique `appid`
3. Add your entry-point `.c` file
4. Add a `README.md` describing the app
