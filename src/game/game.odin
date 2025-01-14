package game

import "core:fmt"
import "src:game/scene"
import "src:game/settings"
import rl "vendor:raylib"

playerCamera: rl.Camera
state: GameState

GameState :: enum {
	MainMenu,
	Loading,
	GameRunning,
	GameOver,
	GamePauseMenu,
}

Initialize :: proc() {
	state = GameState.Loading

	settings.Initialize()

	rl.InitWindow(settings.screenWidth, settings.screenHeight, settings.GameTitle)
	rl.SetTargetFPS(settings.maxFPS)
	rl.SetExitKey(rl.KeyboardKey.KEY_NULL)

	playerCamera = scene.Initialize()
	state = GameState.MainMenu

}

Update :: proc() {
	rl.BeginDrawing()

	rl.ClearBackground(rl.BLACK)

	switch state {
	case GameState.MainMenu:
		MenuUpdate()
	case GameState.Loading:
		fmt.printfln("no loading screen!")
	case GameState.GameRunning:
		rl.BeginMode3D(playerCamera)
		scene.Update(&playerCamera)
		rl.EndMode3D()
		HudUpdate()
	case GameState.GamePauseMenu:
		fmt.printfln("no pause menu yet!")
	case GameState.GameOver:
		// TODO this has to clean up and then remake the same level if player restarts
		fmt.printfln("no gameover yet!")
	}
	GlobalStateUpdate()

	rl.EndDrawing()
}

MenuUpdate :: proc() {
	// TODO We need to do level clean up here
	rl.DisableCursor()
	//https://www.raylib.com/examples/textures/loader.html?name=textures_image_processing
	MenuButton("Press space to start", rl.WHITE, rl.Rectangle{40.0, 50.0, 150.0, 30.0})
	MenuButton("Press esc to quit", rl.WHITE, rl.Rectangle{40.0, 80.0, 150.0, 30.0})

	// menu presses etc come here
	// Enable and disable cursor based on if menu is on or off
	if rl.IsKeyPressed(rl.KeyboardKey.SPACE) {
		state = GameState.GameRunning
	}
	if rl.IsKeyPressed(rl.KeyboardKey.ESCAPE) {
		fmt.printfln("goodbye!")
		rl.CloseWindow()
	}

}

// TODO: state updater that sniffs for inputs like pause menu etc.
GlobalStateUpdate :: proc() {

	if (state == GameState.GamePauseMenu) {
		if rl.IsKeyPressed(rl.KeyboardKey.ESCAPE) {
			fmt.printfln("state set to gamin")
			state = GameState.GameRunning
		}
		if rl.IsKeyPressed(rl.KeyboardKey.Q) {
			fmt.printfln("state set to menu")
			state = GameState.MainMenu
		}
	} else if (state == GameState.GameRunning) {
		if rl.IsKeyPressed(rl.KeyboardKey.ESCAPE) {
			fmt.printfln("state set to pause")
			state = GameState.GamePauseMenu
		}
	}

}

