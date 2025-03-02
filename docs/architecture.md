# Engine Architecture Notes

## Loading Game Libraries

```
+---- Engine ----+             +--- Game -----+
|                |             |              |
|    Launcher    |             |              |
|       V        |             |              |
| Engine Library | <---------> | Game Library |
|  Math Library  | <---------- |              |
+----------------+             +--------------+
```

Assume root path (`/`) is the directory containing the engine launcher.
Engine launcher loads engine library and math library on startup.
It then searches for and loads games from the `/games` directory.

```
/
  launcher
  rayge-engine.so
  rayge-math.so
  games/
    mygame/
      game.json
      ...
    mygame2/
      game.json
      ...
```

Each game is identified by a `game.json` file. This file defines
game-global configuration, including which game libraries to load.

```json
{
	"unique_id": "mygame",
	"display_name": "My Game",
	"client_library": "lib/client",
	"server_library": "lib/server"
}
```

When the engine loads a game library, it requests a pointer to the engine callback struct.
This struct is versioned according to the macro built into the engine header
from which the game library was compiled.

Engine callback header (shared):

```c
#define RAYGE_GAME_CALLBACKS_VERSION 1
#define RAYGE_GAME_CALLBACKS_SENTRY 0xFB15EF4F2AC07B8C

typedef struct RayGE_GameCallbacks
{
	uint64_t version;
	uint64_t beginSentry;

	// Function pointers here
	// ...

	uint64_t endSentry;
} RayGE_GameCallbacks;

static inline void RayGE_GameCallbacks_Init(RayGE_GameCallbacks* callbacks)
{
	if ( !callbacks )
	{
		return;
	}

	*callbacks = (RayGE_GameCallbacks){};
	callbacks->version = RAYGE_GAME_CALLBACKS_VERSION;
	callbacks->beginSentry = RAYGE_GAME_CALLBACKS_SENTRY;
	callbacks->endSentry = ~(RAYGE_GAME_CALLBACKS_SENTRY);
}
```

Game library:

```c
static RayGE_GameCallbacks g_Callbacks;

MYGAME_EXPORT const RayGE_GameCallbacks* __cdecl RayGE_GameCallbacks_Get(void)
{
	RayGE_GameCallbacks_Init(&g_Callbacks);

	// Populate function pointers here...

	return &g_Callbacks;
}
```

Engine library:

```c
typedef const RayGE_GameCallbacks* (__cdecl * GetCallbacksFunc)(void);

const RayGE_GameCallbacks* GetGameCallbacks(void* gameLibrary)
{
	// Platform-appropriate call:
	GetCallbacksFunc funcPtr = dlsym(gameLibrary, "RayGE_GameCallbacks_Get");

	if ( !funcPtr )
	{
		Log(Error, "Could not find RayGE_GameCallbacks_Get function in game library\n");
		return NULL;
	}

	const RayGE_GameCallbacks* callbacks = (*funcPtr)();

	if ( !callbacks )
	{
		Log(Error, "Could not obtain game library callbacks\n");
		return NULL;
	}

	// Easy check for static initialisation of the struct
	// (ie. the user forgot to call RayGE_GameCallbacks_Init())
	if ( callbacks->version == 0 )
	{
		Log(Error, "Game library callbacks struct did not specify a valid version\n");
		return NULL;
	}

	// Check the version matches the version this engine build is using.
	if ( callbacks->version != RAYGE_GAME_CALLBACKS_VERSION )
	{
		Log(Error, "Expected game callbacks version %lu but got %lu\n",
			RAYGE_GAME_CALLBACKS_VERSION,
			callbacks->version);

		return NULL;
	}

	// Check the sentinels are correct, for struct size
	if ( callbacks->beginSentry != RAYGE_GAME_CALLBACKS_SENTRY ||
			callbacks->beginSentry ^ callbacks->endSentry != ~0 )
	{
		Log(Error, "Game library callbacks sentry values were incorrect\n");
		return NULL;
	}

	// Struct is valid.
	return callbacks;
}
```

## Main Engine Loop

Broadly speaking, the engine loop can be split up into the following stages, in order:

1. **Deserialisation** of state (from on-disk resources, network streams, etc).
2. **Input** from user (or simulated users, in the case of bots).
3. **Game logic** to mutate state based on input, and drive physics.
4. **Simulation** of physics based on computed state.
5. **Rendering** of results to the screen.
6. **Serialisation** of state if required, eg. to save files or network streams.

Behavioural systems exist within the engine and fall under one or more of these stages: for example, a physics engine would fall under the **simulation** stage.

Each entity in the scene aggregates components, and each component is an endpoint for plugging the entity into that component's behavioural system. If an entity should be physically simulated, it should hold a physics component; conversely, if no physics component is present on an entity, it will not be affected by the physics simulation.
