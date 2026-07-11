# RipcordAlt

A lightweight, native Discord client. No Electron, no embedded browser,
no GPU rendering path — just Qt Widgets over raw REST + WebSocket calls
to Discord's own API, same protocol the official client uses under the
hood.

## Current feature set

- **Login**: email/password, with MFA prompt and a token-paste fallback
  when Discord throws up a captcha (see design constraints below for why
  we don't try to solve it).
- **Servers & channels**: guild list via REST, channel list per guild via
  the gateway's `GUILD_CREATE` payloads (text channels only for now).
  Because Discord lazy-loads guild data for accounts in many servers, the
  client sends an explicit "guild subscription" request (gateway opcode
  14) the moment you click a server - without this, servers just sit in
  an "unavailable" state forever and their channels never arrive.
- **Messages**: full history load when you open a channel
  (`GET /channels/{id}/messages`), live incoming messages via the
  gateway's `MESSAGE_CREATE` dispatch, and sending
  (`POST /channels/{id}/messages`) with a text box + Send button.
- **Profile editing**: username, bio ("about me"), and avatar (auto
  resized to 256x256 and re-encoded as PNG before upload to keep the
  payload small) via `PATCH /users/@me`. Username changes require your
  current password, exactly as Discord's own settings page does.
- **Status control**: Online / Idle / Do Not Disturb / Invisible, sent
  live over the gateway (`PresenceUpdate`, opcode 3) — takes effect
  immediately, same as the real client's status dot.
- **Rich presence / game detection**: pulls Discord's own public
  `applications/detectable` list (the same data the official client uses
  to know which `.exe` maps to which game), polls the running process
  list every 10 seconds via `CreateToolhelp32Snapshot`, and automatically
  sends a "Playing X" presence update when a match starts or stops
  running. No hooks, no injected code, no GPU — just a process name scan.

- **Voice channels (signaling only, no audio yet)**: clicking a voice
  channel sends a real Voice State Update to Discord's main gateway,
  receives back a voice server assignment, connects to Discord's
  separate voice WebSocket gateway, identifies, and performs the UDP "IP
  discovery" handshake exactly like the real client - ending with a
  genuine `secret_key` from Discord ready to encrypt audio with. No audio
  is captured, encoded, encrypted, or sent yet - that's Phase B/C, and
  needs two new native libraries (Opus for the codec, libsodium for
  encryption) not yet part of this project. Every step writes to
  `gateway-debug.log` (prefixed `[voice]`) so it can be verified
  independently.

## Build (Windows, Qt6, CMake)

```
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
cmake --build . --config Release
```

Required Qt6 modules: Core, Widgets, Network, WebSockets, Sql.

## Design constraints this project holds itself to

- **No GPU, ever.** Only `Qt6::Widgets` is linked — no `Qt6::Quick`, no
  `Qt6::QuickWidgets`, no `Qt6::WebEngineWidgets`. `main.cpp` also forces
  software-rendering env vars as a second line of defense. If a future
  dependency pulls in any GPU-backed module, that's a design violation —
  stop and reconsider before adding it.
- **Memory budget target: under 100 MB.** Current known risk areas for
  this budget, in order of how much they matter:
  1. Avatar images (the profile dialog already downsizes to 256x256
     before upload, but rendering avatars in the member list — not yet
     built — will need an LRU-capped decoded-pixmap cache, not a raw
     dictionary that grows forever).
  2. Message history — currently only the most recent 50 messages per
     channel are fetched, and old channels' messages are dropped from
     memory when you switch away (each `populateChannelsForGuild` call
     clears `m_messageView`). Keep this behavior; don't switch to an
     unbounded in-memory message log.
  3. SQLite local cache (planned, not yet built) — page it, don't load
     entire history into memory at once.
- **No embedded browser for captcha**, by design — see `restclient.h`
  and `logindialog.cpp` for the fallback flow.

## Known gaps / what's still missing

- No Resume support on gateway reconnect — a dropped connection just
  re-Identifies from scratch instead of resuming the session.
- No voice channels yet (the hardest remaining piece — UDP/RTP, Opus,
  libsodium encryption negotiation).
- No member list, no typing indicators, no read receipts, no reactions,
  no attachments/embeds rendering (attachment-only messages currently
  show a placeholder string instead of the actual file/image).
- Category and voice channel *types* are filtered out of the channel
  list pane entirely for now — only text channels (type 0) are shown.
- `ProcessWatcher` is Windows-only, matching the Windows-only project
  scope. It polls every 10 seconds by name match only — it won't catch
  games launched through unusual wrapper processes (e.g. some
  launcher-in-launcher setups), same limitation the real Discord client
  has without its more elaborate detection heuristics.
- Detectable-games list is fetched once at startup and never refreshed;
  a long-running session won't pick up newly-added detectable games
  until restart.

## Risk note (unchanged from planning, repeating here so it stays visible)

This connects using a real user account token rather than a bot token —
against Discord's Terms of Service, with a real risk of account action
on whatever account logs in through it. Product decision made earlier,
not a code issue, but worth re-reading before you actually log in with
an account you care about.
