public class PLAYER_DATA
{
    FLOATING_POINT_PAIR position;
    public double lookAt;
    public double screenHalfToDistanceToScreen;
    public double distanceToScreen;
    public double rotateVelocity;
    public double moveVelocity;

    public PLAYER_DATA(FLOATING_POINT_PAIR position_, double look_at_, double field_of_view_, double rotate_velocity_, double move_velocity_, int window_width_)
    {
        this.position = position_;
        this.lookAt = look_at_;
        this.screenHalfToDistanceToScreen = Math.sin(Math.toRadians(field_of_view_ / 2)) / Math.cos(Math.toRadians(field_of_view_ / 2));
        this.distanceToScreen = window_width_ / 2.0 / this.screenHalfToDistanceToScreen;
        this.rotateVelocity = rotate_velocity_;
        this.moveVelocity = move_velocity_;
    }
}
