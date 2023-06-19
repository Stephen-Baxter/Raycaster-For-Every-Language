import java.lang.Math;
import java.lang.reflect.Method;

public class MAIN
{
    WINDOW_DATA windowData;
    PLAYER_DATA playerData;
    GAME_DATA gameData;

    public void Loop(String message)
    {
        windowData.CleerScreen();

        double playerLookAtX = Math.cos(Math.toRadians(playerData.lookAt));
        double playerLookAtY = Math.sin(Math.toRadians(playerData.lookAt));
        double playerPlaneX = playerLookAtY * playerData.screenHalfToDistanceToScreen;
        double playerPlaneY = -playerLookAtX * playerData.screenHalfToDistanceToScreen;
        FLOATING_POINT_PAIR mapPositionOffset = new FLOATING_POINT_PAIR(playerData.position.x % 1, playerData.position.y % 1);
        FLOATING_POINT_PAIR mapPosition = new FLOATING_POINT_PAIR(Math.floor(playerData.position.x), Math.floor(playerData.position.y));

        for (int i = 0; i < windowData.width; i++)
        {
            double cameraX = 2d * i / windowData.width - 1d;

            FLOATING_POINT_PAIR rayDirection = new FLOATING_POINT_PAIR(playerLookAtX + playerPlaneX * cameraX, playerLookAtY + playerPlaneY * cameraX);
            if (rayDirection.x == 0) {rayDirection.x = 0.000001;}
            if (rayDirection.y == 0) {rayDirection.y = 0.000001;}
            
            FLOATING_POINT_PAIR playerMapPositionForDDA = new FLOATING_POINT_PAIR(mapPosition.x, mapPosition.y);
            FLOATING_POINT_PAIR step = new FLOATING_POINT_PAIR(0, 0);
            FLOATING_POINT_PAIR deltaDistance = new FLOATING_POINT_PAIR(Math.abs(1 / rayDirection.x), Math.abs(1 / rayDirection.y));
            FLOATING_POINT_PAIR sideDistance = new FLOATING_POINT_PAIR(0, 0);
            double distanceToWall = 0;
            boolean wallSideX = true;

            if (rayDirection.x < 0)
            {
                step.x = -1d;
                sideDistance.x = mapPositionOffset.x * deltaDistance.x;
            }
            else
            {
                step.x = 1;
                sideDistance.x = (1 - mapPositionOffset.x) * deltaDistance.x;
            }
            if (rayDirection.y < 0)
            {
                step.y = -1;
                sideDistance.y = mapPositionOffset.y * deltaDistance.y;
            }
            else
            {
                step.y = 1;
                sideDistance.y = (1 - mapPositionOffset.y) * deltaDistance.y;
            }

            while (true)
            {
                if (sideDistance.x < sideDistance.y)
                {
                    sideDistance.x += deltaDistance.x;
                    playerMapPositionForDDA.x += step.x;
                    wallSideX = true;
                }
                else
                {
                    sideDistance.y += deltaDistance.y;
                    playerMapPositionForDDA.y += step.y;
                    wallSideX = false;
                }

                if (gameData.GetMapData((int)playerMapPositionForDDA.x, (int)playerMapPositionForDDA.y) == 1) break;
            }

            if (wallSideX) distanceToWall = sideDistance.x - deltaDistance.x;
            else distanceToWall = sideDistance.y - deltaDistance.y;

            double wallHeight = 1 / distanceToWall * playerData.distanceToScreen;

            int ceiling = (int)Math.floor(windowData.height / 2 - wallHeight / 2);
            if (ceiling < 0) ceiling = 0;
            int floor = (int)Math.floor(windowData.height / 2 + wallHeight / 2);
            if (floor >= windowData.height) floor = windowData.height - 1;
            
            windowData.DrawLine(0x0000FF, i, ceiling, floor);
        }
        windowData.DrawScreen();
        
        double playerRotateVelocity = playerData.rotateVelocity * windowData.elapsedTime;
        double playerMoveVelocity = playerData.moveVelocity * windowData.elapsedTime;
        if (windowData.KeyPress(37))//left
        {
            playerData.lookAt = playerData.lookAt + playerRotateVelocity;
            if (playerData.lookAt >= 360) playerData.lookAt = playerData.lookAt - 360;
        }
        if (windowData.KeyPress(39))//right
        {
            playerData.lookAt = playerData.lookAt - playerRotateVelocity;
            if (playerData.lookAt < 0) playerData.lookAt = playerData.lookAt + 360;
        }
        if (windowData.KeyPress(38))//up
        {
            double newPositionX = playerData.position.x + playerLookAtX * playerMoveVelocity;
            double newPositionY = playerData.position.y + playerLookAtY * playerMoveVelocity;
            if (Math.floor(newPositionX) >= 0 && Math.floor(newPositionX) < gameData.mapWidth && Math.floor(newPositionY) >= 0 & Math.floor(newPositionY) < gameData.mapWidth)
            {
                if (gameData.GetMapData((int)Math.floor(newPositionX), (int)Math.floor(newPositionY)) == 0)
                {
                    playerData.position.x = newPositionX;
                    playerData.position.y = newPositionY;
                }
            }
        }
        if (windowData.KeyPress(40))//down
        {
            double newPositionX = playerData.position.x - playerLookAtX * playerMoveVelocity;
            double newPositionY = playerData.position.y - playerLookAtY * playerMoveVelocity;
            if (Math.floor(newPositionX) >= 0 && Math.floor(newPositionX) < gameData.mapWidth && Math.floor(newPositionY) >= 0 && Math.floor(newPositionY) < gameData.mapWidth)
            {
                if (gameData.GetMapData((int)Math.floor(newPositionX), (int)Math.floor(newPositionY)) == 0)
                {
                    playerData.position.x = newPositionX;
                    playerData.position.y = newPositionY;
                }
            }
        }
    }

    public MAIN()
    {
        this.windowData = new WINDOW_DATA(400, 300, "Ray Caster");
        this.playerData = new PLAYER_DATA(new FLOATING_POINT_PAIR(5.0, 5.0), 0, 60, 100.0, 5.0, windowData.width);
        this.gameData = new GAME_DATA("1111111111100000000110100001011000000001100000000110000000011000000001101000010110000000011111111111", 10);
    }
    public static void main(String[] args) throws NoSuchMethodException, NullPointerException, SecurityException, Exception
    {
        Class[] parameterTypes = new Class[1];
        parameterTypes[0] = String.class;
        Method loop = MAIN.class.getMethod("Loop", parameterTypes[0]);
        MAIN main = new MAIN();
        main.windowData.Start(main, loop);
    }
}
