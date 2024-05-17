Modules are designed to be containers which manage engine-static resources. This means:

* A module is initialised when the engine starts, and shut down when the engine shuts down.
* A module can initialise data on startup, and destroy it on shutdown. This is intended for data which should live
  for the entire lifetime of the engine.
* A module can expose its data for use by other parts of the engine.
