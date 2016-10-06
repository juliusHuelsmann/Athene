package spotlight.delphi.view;


import java.awt.AWTException;
import java.awt.Color;
import java.awt.Component;
import java.awt.Point;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.io.IOException;
import java.util.logging.Logger;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
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
public class Overlay extends JWindow {
 
  
  private JFrame jf;
  
  public void setTransparent() {

    // Apply a transparent color to the background
    // This is ALL important, without this, it won't work!
//    final 
    int alpha;
    final boolean windows = UtilitiesSystem.isWindows();
    if (windows) {
      alpha = 1;
    } else {
      alpha = 0;
    }
//    alpha += 50;
    
    super.setBackground(new Color(100, 120, 90, alpha));
  }
  
  
  public Overlay() {
    

    // Apply a transparent color to the background
    // This is ALL important, without this, it won't work!
//    setUndecorated(true);
    setVisible(true);
    setTransparent();
    setSize(Toolkit.getDefaultToolkit().getScreenSize());

    
    
    MousePressFetcher mpf = new MousePressFetcher(this);
    super.addMouseListener(mpf);
    super.addMouseMotionListener(mpf);
    super.addMouseWheelListener(mpf);
    setAlwaysOnTop(true);
    super.setFocusable(false);
    
    
    jf = new JFrame("Spotlight");
    jf.setUndecorated(true);
    jf.setVisible(true);
    jf.setAlwaysOnTop(true);
    jf.setSize(30, 30);
    jf.setBackground(new Color(255, 255, 255, 100));
    jf.setLocation(getWidth() - jf.getWidth(), getHeight() - jf.getHeight());
    jf.setResizable(false);
    jf.setFocusable(false);
    
  }
    public static void main( String[] args ) {
    	new Overlay();
    }


    /**
     * @return the jf
     */
    public JFrame getJf() {
      return jf;
    }

}


class MousePressFetcher implements MouseListener, MouseWheelListener, MouseMotionListener, ClipboardOwner {

  
  /**
   * An instance of the view class.
   */
  private Overlay ol;
  
  
  
  /**
   * The robot that is required for forewarding the clicks.
   */
  private Robot r ;
  
  
  
  

  
  /**
   * The timestamp that is recorded on press.
   */
  private long pTimestamp = 0;
  
  /**
   * The timestamp that is recorded on release
   */
  private long rTimestamp = 0;
  
  
  /**
   * The difference that is used for considering click as a word-lookup.
   */
  private long mindifference = 1000;
  

  private boolean dragged = false;
  
  
  private Thread tMoveMouseWheel;
  private Thread t_displayTimeout;

  
  
  
  
  /**
   * Constructor: save the Overlay and initiate the values.
   * @param xol
   */
  public MousePressFetcher(final Overlay xol) {
    this.ol = xol;
    try {
      r = new Robot();
      
    } catch (AWTException e) {
      e.printStackTrace();
    }
  }

  
  private String copytoclipboard() {
    
    // check if clipboard copy
  
  Clipboard c = Toolkit.getDefaultToolkit().getSystemClipboard();
  final Transferable oldData =  c.getContents(this);
  
  r.keyPress(KeyEvent.VK_CONTROL); // Copy
  r.keyPress(KeyEvent.VK_C);
  r.keyRelease(KeyEvent.VK_C);
  r.keyRelease(KeyEvent.VK_CONTROL);
  
  final Transferable newData =  c.getContents(this);
  String nd = "";
  try {
    nd = "" + newData.getTransferData(DataFlavor.stringFlavor);
  } catch (UnsupportedFlavorException e1) {
  } catch (IOException e1) {
  }
  c.setContents(oldData, MousePressFetcher.this);
  return nd;
  }

  
  //TODO: check if mouse has been moved. In that case, exit the oepration
    public void mousePressed(MouseEvent e) {
    	

  	  Logger.getLogger("Spotlight").severe("mp");
      // Initialize with not dragged.
      dragged = false;
      pTimestamp = System.currentTimeMillis();
      
      t_displayTimeout = new Thread() {
        
        public void interrupt() {
          super.interrupt();
          ol.getJf().setBackground(new Color(255, 255, 255, 100));
        }
        
        public void run() {
          try {
            if (!isInterrupted()) {
  
              Thread.sleep(mindifference / 2);
              
  
              try {
                final int sprung = 10;
                for (int i = 0; i < 255 && !isInterrupted(); i+=sprung) {
                  ol.getJf().setBackground(new Color(255 - i, i, 0, 100));
                  Thread.sleep(Math.max(1, mindifference * sprung / 255 / 2));
                }
              } catch (InterruptedException e) {
  //              super.interrupt();
              }
            }
          } catch (InterruptedException e1) {
          }
  
          ol.getJf().setBackground(new Color(255, 255, 255, 100));
        }
      };
      t_displayTimeout.start();
    }



  public void mouseReleased(final MouseEvent e) {
	  

	  Logger.getLogger("Spotlight").severe("MR");
	  rTimestamp = System.currentTimeMillis();
	  if (dragged) {
        ol.setVisible(false);
    
        r.mouseRelease(getButton(e));
        dragged = false;
        
        ol.setVisible(true);
        rTimestamp = 0;
        pTimestamp = 0;
        
        if (t_displayTimeout != null) {
            t_displayTimeout.interrupt();
        }
      } 

    }



  /**
   * set FullscreenMode.
   */
  public void mouseClicked(final MouseEvent e) {

	  Logger.getLogger("Spotlight").severe("mc\n\n");
    if (dragged) {
      return;
    } else {

      if (rTimestamp - pTimestamp > mindifference) {
        // action (look up word)
        t_displayTimeout.interrupt();


        final String s = copytoclipboard();
        
        if (s.equals("")) {

            System.out.println("\t[LOC]\t(" + e.getXOnScreen() + ", " + e.getYOnScreen() + ")");
        } else {

            System.out.println("\t[STR]\t(" + e.getXOnScreen() + ", " + e.getYOnScreen() + ")\t\"" + s + "\"");	
        }

//        ol.setTransparent();
      } else {
      	System.out.println("foreward");
        // no action, let the robot take care of this
        t_displayTimeout.interrupt();
        ol.setTransparent();

        ol.setVisible(false);
        
        
        final int d = getButton(e);
        try {
			Thread.sleep(10);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
        r.mousePress(d);
        r.mouseRelease(d);

        try {
			Thread.sleep(10);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
        ol.setVisible(true);
      }
    }
    pTimestamp = 0;
    rTimestamp = 0;
  }  
  
  
  
  public void mouseWheelMoved(final MouseWheelEvent e) {
      if (tMoveMouseWheel != null) {
        tMoveMouseWheel.interrupt();
        tMoveMouseWheel = null;
      }
      ol.setVisible(false);
      //TODO: make sure that all threads are stored globally and may be killed
      tMoveMouseWheel = new Thread () {
        public void run(){
          try {
            r.mouseWheel(3 * e.getWheelRotation());
            Thread.sleep(500);
            for (int i = 5; i > 0; i--) {

            }
            ol.setVisible(true);
//            setVisible(true);
          } catch (InterruptedException e1) {
            ol.setVisible(true);}
        }
      };
      tMoveMouseWheel.run();
      
    }
  
  

  public void mouseEntered(MouseEvent e) { }
  public void mouseExited(MouseEvent e) { }
  public void mouseDragged(MouseEvent e) {
	  
    
//    if (t_displayTimeout != null) {
//
//        t_displayTimeout.interrupt();
//        t_displayTimeout = null;
//    }
//      animation.setVisible(false);
	  if (!dragged) {
		    dragged = true;
		  System.out.println("md");
	      r.mouseRelease(getButton(e));
	      try {
			Thread.sleep(100);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
	      ol.setVisible(false);
	      
	      try {
			Thread.sleep(100);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}

	      r.mousePress(getButton(e));
	      try {
			Thread.sleep(100);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
	      ol.setVisible(true);
	  }
  }
  public void mouseMoved(MouseEvent e) { }

  private final static int getButton(final MouseEvent e) {

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
    return val;
  }



  public void lostOwnership(Clipboard clipboard, Transferable contents) {
    // TODO Auto-generated method stub
    
  }
}


