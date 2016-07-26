package informationWindow.informationWindow;

import java.awt.AWTEvent;
import java.awt.AWTException;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.Insets;
import java.awt.MouseInfo;
import java.awt.Point;
import java.awt.PointerInfo;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.image.BufferedImage;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JWindow;
/**
 * Hello world!
 *
 */
public class App extends JWindow implements MouseListener, MouseMotionListener, MouseWheelListener {
 
  private AnimationWindow animation;
  
  /**
   * Image Tupel that is recorded on click.
   */
  private ImageTupel it;
  
  /**
   * Integer which is used for identifying whether the current image tupel
   * is containing the latest data.
   */
  private int currentClicknumber;
  
  public App() {
    try {
      r = new Robot();
    } catch (AWTException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    }
//    setUndecorated(true);
 // Apply a transparent color to the background
 // This is ALL important, without this, it won't work!
    setBackground(new Color(0, 255, 0, 1));
    setVisible(true);
    setSize(Toolkit.getDefaultToolkit().getScreenSize());
    addMouseListener(this);
    addMouseMotionListener(this);
    super.addMouseWheelListener(this);
    setAlwaysOnTop(true);
    setFocusable(false);
//    setFullscreen();
    animation = new AnimationWindow();
    
//    for(int i = 0; i < 100; i++) {
//
//      PointerInfo inf = MouseInfo.getPointerInfo();
//      if (inf != null) {
//
//        Point p = inf.getLocation();
//      }
//      try {
//        Thread.sleep(90);
//      } catch (InterruptedException e) {
//        e.printStackTrace();
//      }
//    }
//    System.exit(1);
  
  }
    public static void main( String[] args )
    {new App();}

    private Robot r ;
    
    /**
     * set FullscreenMode.
     */
    public void mouseClicked(MouseEvent e) {
      System.out.println(pTimestamp);
      System.out.println(rTimestamp);
      
      if (rTimestamp - pTimestamp > mindifference) {
        // action (look up word)
        System.out.println("noclick");
       
      } else {
        // no action, let the robot take care of this
        System.out.println("click");
        setVisible(false);
        
        final int val;
        switch(e.getButton()) {
        case MouseEvent.BUTTON1:
          val = InputEvent.BUTTON1_DOWN_MASK;
          break;
        case MouseEvent.BUTTON2:
          val = InputEvent.BUTTON2_DOWN_MASK;
          break;
        case MouseEvent.BUTTON3:
          val = InputEvent.BUTTON3_DOWN_MASK;
          break;
        default:
        	val = InputEvent.BUTTON1_DOWN_MASK;
        }
 
        new Thread() {
        	public void run() {

                r.mousePress(val);
                r.mouseRelease(val);   
                r.waitForIdle();
        	}
 
        } .start();
        	
        try {
			Thread.sleep(200);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
        setVisible(true);
      }
      
      pTimestamp = 0;
      rTimestamp = 0;
    }
    private long pTimestamp = 0;
    private long rTimestamp = 0;
    private long mindifference = 200;
    
    
    //TODO: check if mouse has been moved. In that case, exit the oepration
    public void mousePressed(MouseEvent e) {
      System.out.println(e.getSource());
      BufferedImage bi = r.createScreenCapture(getBounds());
      currentClicknumber++;
      it = new ImageTupel(bi, currentClicknumber);
      savedLocation = e.getLocationOnScreen();
      pTimestamp = System.currentTimeMillis();
      animation.prepare(
          e.getX() - animation.getWidth() / 2,
          e.getY() - animation.getHeight() / 2,
          e.getX() - animation.getWidth() / 2,
          e.getYOnScreen() - animation.getHeight() / 2, mindifference, bi);
      t_displayTimeout = new Thread() {
        public void run() {
          try {
            Thread.sleep(300);
            if (!isInterrupted()) {
          	  System.out.println("launch animation");

              animation.setVisible(true);
            }
          } catch (InterruptedException e) {
          }
        }
      };
      t_displayTimeout.start();
    }
    private Thread t_displayTimeout;
    public void mouseReleased(MouseEvent e) {
      rTimestamp = System.currentTimeMillis();
  	t_displayTimeout.interrupt();
      animation.setVisible(false);
      setVisible(true);
    }
    public void mouseEntered(MouseEvent e) {
      
    }
    public void mouseExited(MouseEvent e) {
      
    }
    public void mouseDragged(MouseEvent e) {
      if (e.getLocationOnScreen().equals(savedLocation)) {
    	  System.out.println("interrupt");
        t_displayTimeout.interrupt();
        animation.setVisible(false);
        it = null;
        setVisible(false);
      } else {
    	  System.out.println("does not equal");
      }
    
    }
    private Point savedLocation;
    public void mouseMoved(MouseEvent e) {}
    private Thread t;
    public void mouseWheelMoved(final MouseWheelEvent e) {
      System.out.println("mousewehelmoved");
      if (t != null) {
        t.interrupt();
        t = null;
      }
      setVisible(false);
      //TODO: make sure that all threads are stored globally and may be killed
      t = new Thread () {
        public void run(){
          try {
            r.mouseWheel(3 * e.getWheelRotation());
            Thread.sleep(500);
            for (int i = 5; i > 0; i--) {

            }
            setVisible(true);
//            setVisible(true);
          } catch (InterruptedException e1) {
            setVisible(true);}
        }
      };
      t.run();
      
    }
}
