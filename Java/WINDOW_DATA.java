import java.lang.reflect.Method;
import java.util.Arrays;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.lang.Thread;

public class WINDOW_DATA implements KeyListener
{
    public int width;
    public int height;
    public boolean[] keysBuffer;

    private BufferedImage screen;
    private Graphics2D g;
    private JFrame window;

    private long timePointOne;
    private long timePointTwo;
    public double elapsedTime;
    private double fps;
    private long spf;

    public WINDOW_DATA(int width_, int height_, String title_)
    {
        this.width = width_;
        this.height = height_;
        this.screen = new BufferedImage(width_, height_, BufferedImage.TYPE_INT_RGB);
        this.g = screen.createGraphics();
        this.window = new JFrame();
        this.window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.window.setTitle(title_);
        this.window.setSize(width_, height_);
        this.window.setVisible(true);

        this.keysBuffer = new boolean[256];
        Arrays.fill(keysBuffer, false);
        this.window.addKeyListener(this);

        this.timePointOne = System.currentTimeMillis();
        this.timePointTwo = System.currentTimeMillis();
        this.elapsedTime = 0.0;
        this.fps = 60.0;
        this.spf = (long)(int)(1000.0 / this.fps);
    }

    public boolean KeyPress(int key_code_)
    {
        return this.keysBuffer[key_code_];
    }

    public void keyPressed(KeyEvent key)
    {
        this.keysBuffer[key.getKeyCode()] = true;
    }

    public void keyTyped(KeyEvent key) {}

    public void keyReleased(KeyEvent key)
    {
        this.keysBuffer[key.getKeyCode()] = false;
    }
    
    public void CleerScreen()
    {
        this.g.setPaint(new Color(198, 198, 198));
        this.g.fillRect(0, 0, width, height);
    }

    public void DrawLine(int color_, int x_, int y_one_, int y_two_)
    {
        this.g.setPaint(new Color(0, 0, 255));
        this.g.drawLine(x_, y_one_, x_, y_two_);
    }

    public void DrawScreen()
    {
        ImageIcon oi = new ImageIcon(screen);
        JLabel jl = new JLabel(oi);
        window.add(jl);
        window.pack();
    }

    public void UpdateElapsedTime() throws InterruptedException, IllegalArgumentException
    {
        if (System.currentTimeMillis() < this.timePointOne + spf)
        {
            Thread.sleep(this.timePointOne + spf - System.currentTimeMillis());
        }
        this.timePointTwo = System.currentTimeMillis();
        long elapse = this.timePointTwo - this.timePointOne;
        this.elapsedTime = elapse / 1000.0; 
        this.timePointOne = System.currentTimeMillis();
    }

    public void Start(Object object, Method method) throws Exception
    {
        while(true)
        {
            method.invoke(object, new Object[1]);
            UpdateElapsedTime();
        }
        
    }

    /*public void Start(Object object, Method method) throws Exception
    {
        long last = System.nanoTime();
        final double ns = 1000000000.0 / 60.0;
        double delta = 0;
        window.requestFocus();
        while(true)
        {
            long now = System.nanoTime();
            delta = delta + ((now-last)/ns);
            while(delta >= 1)
            {
                method.invoke(object, new Object[1]);
                
                delta--;
            }
        }
        
    }*/
}