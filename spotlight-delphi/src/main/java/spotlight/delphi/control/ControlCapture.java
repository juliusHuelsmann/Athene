package spotlight.delphi.control;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import spotlight.delphi.model.SettingsDelphi;
import spotlight.delphi.view.Overlay;


/**
 * The controller class that handles the capturing of an entire word or
 * of the point at which the word is located - according to the user-defined
 * settings #CaptureSettings
 * 
 * @author Julius Huelsmann
 * @since 1.0
 * @version %I%, %U%
 */
public class ControlCapture implements KeyListener {
  
  
  
  /**
   * The view overlay class.
   */
  private Overlay ol;
  
  /**
   * 
   */
  private SettingsDelphi sd;
  
  
  public ControlCapture(final SettingsDelphi xsd) {
    ol = new Overlay();
    ol.addKeyListener(this);
  }
  
  
  
  public static void main(String[] args) {
    new ControlCapture(new SettingsDelphi());
  }



  public void keyTyped(KeyEvent e) {
    // TODO Auto-generated method stub
    
  }



  public void keyPressed(KeyEvent e) {
    System.out.println(e);
  }



  public void keyReleased(KeyEvent e) {
    // TODO Auto-generated method stub
    
  }
  
}
