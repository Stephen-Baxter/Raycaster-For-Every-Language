import tkinter
import math
import time
from tkinter.constants import NUMERIC

class POINT:
    def __init__(self_, x_, y_):
        self_.x = x_
        self_.y = y_

class WINDOW_DATA:
    def __init__(self_, width_, height_, title_):
        self_.width = width_
        self_.height = height_

        
        self_.root = tkinter.Tk()
        self_.root.title(title_)

        self_.keysBuffer = []
        self_.elapsedTime = 1
        self_.fps = 60.0
        self_.timePointOne = time.time()
        self_.timePointTwo = time.time()
        self_.spf = (int)(1000.0 / self_.fps)

        for i in range(255): self_.keysBuffer.append(False)
        def KeyDown(event_): self_.keysBuffer[event_.keycode] = True
        def KeyUp(event_): self_.keysBuffer[event_.keycode] = False
        self_.root.bind("<Key>", KeyDown)
        self_.root.bind("<KeyRelease>", KeyUp)

        self_.canvas = tkinter.Canvas(self_.root, bg = "#BDBDBD", height=height_, width=width_)

    def UpdateElapsedTime(self_):
        self_.timePointTwo = time.time()
        self_.elapsedTime = self_.timePointTwo - self_.timePointOne - 0.02
        self_.timePointOne = time.time()
        
    def Start(self_, app_loop_):
        def Loop():
            app_loop_()
            self_.UpdateElapsedTime()
            self_.root.after(self_.spf, Loop)

        self_.root.after(self_.spf, Loop)
        self_.root.mainloop()

    def CleerScreen(self_): self_.canvas.delete("all")
    def DrawLine(self_, color_, x_, y_one_, y_two_): self_.canvas.create_line(x_, y_one_, x_, y_two_, fill = color_)
    def DrawScreen(self_): self_.canvas.pack()

    def KeyPress(self_, key_code_): return self_.keysBuffer[key_code_]

class PLAYER_DATA:
    def __init__(self_, position_, look_at_, field_of_view_, rotate_velocity_, move_velocity_, window_width_):
        self_.position = position_
        self_.lookAt = look_at_
        self_.fieldOfView = field_of_view_
        self_.screenHalfToDistanceToScreen = math.sin(math.radians(field_of_view_ / 2)) / math.cos(math.radians(field_of_view_ / 2))
        self_.distanceToScreen = window_width_ / 2.0 / self_.screenHalfToDistanceToScreen
        self_.rotateVelocity = rotate_velocity_
        self_.moveVelocity = move_velocity_

class GAME_DATA:
    def __init__(self_, map_, map_width_):
        self_.map = map_
        self_.mapWidth = map_width_
        
    def GetMapData(self_, x_, y_): return ord(self_.map[y_ * self_.mapWidth + x_]) - 48

def main():
    windowData = WINDOW_DATA(400, 300, "Ray Caster")
    playerData = PLAYER_DATA(POINT(5, 5), 0, 60, 100.0, 5.0, windowData.width)
    gameData = GAME_DATA("1111111111100000000110100001011000000001100000000110000000011000000001101000010110000000011111111111", 10)

    def Raycaster():
        windowData.CleerScreen()

        playerLookAtX = math.cos(math.radians(playerData.lookAt))
        playerLookAtY = math.sin(math.radians(playerData.lookAt))
        playerPlaneX = playerLookAtY * playerData.screenHalfToDistanceToScreen
        playerPlaneY = -playerLookAtX * playerData.screenHalfToDistanceToScreen
        mapPositionOffset = POINT(playerData.position.x % 1, playerData.position.y % 1)
        mapPosition = POINT(math.floor(playerData.position.x), math.floor(playerData.position.y))

        for i in range(windowData.width):
            cameraX = 2 * i / windowData.width - 1

            rayDirection = POINT(playerLookAtX + playerPlaneX * cameraX, playerLookAtY + playerPlaneY * cameraX)
            if rayDirection.x == 0: rayDirection.x = 0.000001
            if rayDirection.y == 0: rayDirection.y = 0.000001

            playerMapPositionForDDA = POINT(mapPosition.x, mapPosition.y)
            step = POINT(0, 0)
            deltaDistance = POINT(abs(1 / rayDirection.x), abs(1 / rayDirection.y))
            sideDistance = POINT(0, 0)
            distanceToWall = 0
            wallSideX = True

            if rayDirection.x < 0:
                step.x = -1
                sideDistance.x = mapPositionOffset.x * deltaDistance.x
            else:
                step.x = 1
                sideDistance.x = (1 - mapPositionOffset.x) * deltaDistance.x
            if rayDirection.y < 0:
                step.y = -1
                sideDistance.y = mapPositionOffset.y * deltaDistance.y
            else:
                step.y = 1
                sideDistance.y = (1 - mapPositionOffset.y) * deltaDistance.y

            while True:
                if sideDistance.x < sideDistance.y:
                    sideDistance.x += deltaDistance.x
                    playerMapPositionForDDA.x += step.x
                    wallSideX = True
                else:
                    sideDistance.y += deltaDistance.y
                    playerMapPositionForDDA.y += step.y
                    wallSideX = False

                if gameData.GetMapData(playerMapPositionForDDA.x, playerMapPositionForDDA.y) == 1: break

            if wallSideX: distanceToWall = sideDistance.x - deltaDistance.x
            else: distanceToWall = sideDistance.y - deltaDistance.y

            wallHeight = 1 / distanceToWall * playerData.distanceToScreen

            ceiling = math.floor(windowData.height / 2 - wallHeight / 2)
            if ceiling < 0: ceiling = 0
            floor = math.floor(windowData.height / 2 + wallHeight / 2)
            if floor >= windowData.height: floor = windowData.height - 1

            windowData.DrawLine("#0000FF", i, ceiling, floor)

        
        playerRotateVelocity = playerData.rotateVelocity * windowData.elapsedTime
        playerMoveVelocity = playerData.moveVelocity * windowData.elapsedTime
        if windowData.KeyPress(37):#left
            playerData.lookAt = playerData.lookAt + playerRotateVelocity
            if playerData.lookAt >= 360: playerData.lookAt = playerData.lookAt - 360

        if windowData.KeyPress(39):#right
            playerData.lookAt = playerData.lookAt - playerRotateVelocity
            if playerData.lookAt >= 360: playerData.lookAt = playerData.lookAt + 360

        if windowData.KeyPress(38):#up
            newPositionX = playerData.position.x + playerLookAtX * playerMoveVelocity
            newPositionY = playerData.position.y + playerLookAtY * playerMoveVelocity
            if math.floor(newPositionX) >= 0 and math.floor(newPositionX) < gameData.mapWidth and math.floor(newPositionY) >= 0 and math.floor(newPositionY) < gameData.mapWidth:
                if gameData.GetMapData(math.floor(newPositionX), math.floor(newPositionY)) == 0:
                    playerData.position.x = newPositionX
                    playerData.position.y = newPositionY

        if windowData.KeyPress(40):#down
            newPositionX = playerData.position.x - playerLookAtX * playerMoveVelocity
            newPositionY = playerData.position.y - playerLookAtY * playerMoveVelocity
            if math.floor(newPositionX) >= 0 and math.floor(newPositionX) < gameData.mapWidth and math.floor(newPositionY) >= 0 and math.floor(newPositionY) < gameData.mapWidth:
                if gameData.GetMapData(math.floor(newPositionX), math.floor(newPositionY)) == 0:
                    playerData.position.x = newPositionX
                    playerData.position.y = newPositionY

        windowData.DrawScreen()

    windowData.Start(Raycaster)

if __name__ == '__main__': main()
