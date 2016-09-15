package spotlight.delphi.view;


import java.awt.AWTException;
import java.awt.Color;
import java.awt.Point;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

import javax.swing.JWindow;

import utils.UtilitiesSystem;


/**
 * The overlay window that is used for capturing the clicks.
 * Is acting according to the settings specified inside the SettingsDelphi 
 * class.
 * 
 *
 * @author Julius Huelsmann
 * @version %I%, %U%
 * @since 0.0
 */
@SuppressWarnings("serial")
public class Overlay extends JWindow implements MouseListener, 
MouseMotionListener, MouseWheelListener {
 
  
  /**
   * The window which displays the animation.
   */
//  private AnimationWindow animation;
  
  /**
   * Image Tupel that is recorded on click.
   */
//  private ImageTupel it;
  
  /**
   * Integer which is used for identifying whether the current image tupel
   * is containing the latest data.
   */
  private int currentClicknumber;
  
  public Overlay() {
    

    try {
      r = new Robot();
    } catch (AWTException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    }
//    setUndecorated(true);
 // Apply a transparent color to the background
 // This is ALL important, without this, it won't work!
    final int alpha;
    final boolean windows = UtilitiesSystem.isWindows();
    if (windows) {
      alpha = 1;
    } else {
      alpha = 0;
    }
    setBackground(new Color(0, 255, 0, alpha));
//    setVisible(true);
    setSize(Toolkit.getDefaultToolkit().getScreenSize());
    addMouseListener(this);
    addMouseMotionListener(this);
    super.addMouseWheelListener(this);
    setAlwaysOnTop(true);
    setFocusable(false);

//    animation = new AnimationWindow();
//    animation.setLocationRelativeTo(null);

    
    setVisible(true);
//    setFullscreen();
    
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
    {new Overlay();}

    private Robot r ;
    
    /**
     * set FullscreenMode.
     */
    public void mouseClicked(final MouseEvent e) {
      
      if (dragged) {
        return;
      }
      
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
            try {
              Thread.sleep(100);
            } catch (InterruptedException e1) {
              // TODO Auto-generated catch block
              e1.printStackTrace();
            }
            
            r.mouseMove(e.getXOnScreen(), e.getYOnScreen());
            r.mousePress(val);
            r.mouseRelease(val);   
            final long s = System.currentTimeMillis();
            r.waitForIdle();
                
            setVisible(true);
          }
        } .start();
          
      }
      
      pTimestamp = 0;
      rTimestamp = 0;
    }
    private long pTimestamp = 0;
    private long rTimestamp = 0;
    private long mindifference = 200;
    
    
    //TODO: check if mouse has been moved. In that case, exit the oepration
    public void mousePressed(MouseEvent e) {
      
      dragged = false;
      setVisible(false); // if linux that is necessary
//      final BufferedImage bi = r.createScreenCapture(getBounds());
      setVisible(true);
      currentClicknumber++;
//      it = new ImageTupel(bi, currentClicknumber);
      savedLocation = e.getLocationOnScreen();
      pTimestamp = System.currentTimeMillis();
//      animation.prepare(
//          e.getX() - animation.getWidth() / 2,
//          e.getY() - animation.getHeight() / 2,
//          e.getX() - animation.getWidth() / 2,
//          e.getYOnScreen() - animation.getHeight() / 2, mindifference, 
//          getBounds().width, getBounds().height);
      t_displayTimeout = new Thread() {
        public void run() {
          try {
            Thread.sleep(900);
            if (!isInterrupted()) {
              System.out.println("launch animation");

//              animation.setVisible(true);
            }
          } catch (InterruptedException e) {
          }
        }
      };
      t_displayTimeout.start();
    }
    private Thread t_displayTimeout;
    public void mouseReleased(MouseEvent e) {
      if (dragged) {
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
        r.mousePress(val);
        setVisible(true);
      } else {

        rTimestamp = System.currentTimeMillis();
        t_displayTimeout.interrupt();
//        animation.setVisible(false);
        setVisible(true);
        System.out.println("mousereleased");
      }
    }
    public void mouseEntered(MouseEvent e) {
      
    }
    public void mouseExited(MouseEvent e) {
      
    }
    public void mouseDragged(MouseEvent e) {
      dragged = true;
        t_displayTimeout.interrupt();
//        animation.setVisible(false);
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
        r.mousePress(val);
        setVisible(true);
    }
    private boolean dragged = false;
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
