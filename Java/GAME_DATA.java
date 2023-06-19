public class GAME_DATA
{
    String map;
    int mapWidth;

    public GAME_DATA(String map_, int map_width_)
    {
        this.map = map_;
        this.mapWidth = map_width_;
    }

    public int GetMapData(int x_, int y_)
    {
        return Character.getNumericValue(this.map.charAt(y_ * this.mapWidth + x_));
    }
}
