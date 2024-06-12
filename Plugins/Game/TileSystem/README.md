## How to use
### Actor Setup
1. Create an actor class that has the `TileSystem` component. This actor will be the parent of all of the tiles in the system.
2. Create a blueprint class that subclasses the `Tile` class. This will be one of your tiles.
3. Fill in an adjacency match in the details panel. Set 255 for this tile to appear if all of the neighbors are filled in. (NOTE: This adjacency match code will be changed later)
4. Add the `Tile` class to your first actor's `Tilesystem` component under `Tile Actors` -> `Tile Types`

### Usage
1. Place the actor with the `TileSystem` component into the world.
2. Switch to the `Tile System Editor Mode` with the actor selected.
3. Start dragging with left click to fill out the tile system in world.
