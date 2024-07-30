package game

import rl "vendor:raylib"

camera : rl.Camera
isStarted : bool = false

initialize :: proc()
{
    isStarted = false

    // settings.initialize
    //camera = //scene camera

    isStarted = true

}

update :: proc()
{
    rl.BeginDrawing()

    rl.ClearBackground(rl.BLACK)

    rl.BeginMode3D(camera)

    if (isStarted){
        // player update
        // scene update
    }
    rl.EndMode3D()

    //game hudupdate
    // game menuupdate
    rl.EndDrawing()
}

hudUpdate :: proc()
{
    // hud draw
}

menuUpdate :: proc()
{
    rl.DisableCursor()

    // menu presses etc come here
    // Enable and disable cursor based on if menu is on or off
}
