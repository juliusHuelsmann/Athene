package spotlight.delphi.utils;
import java.awt.AWTException;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JFrame;
import javax.swing.JLabel;

import lc.kra.system.mouse.GlobalMouseHook;
import lc.kra.system.mouse.event.GlobalMouseAdapter;
import lc.kra.system.mouse.event.GlobalMouseEvent;

public class Shit {
    private static boolean run = true;
    public static void main(String[] args) {
      

      Robot robot = null;
      try {
        robot = new Robot();
      } catch (AWTException e1) {
        // TODO Auto-generated catch block
        e1.printStackTrace();
      }
      robot.keyPress(KeyEvent.VK_CONTROL); // Copy
      robot.keyPress(KeyEvent.VK_C);
      robot.keyRelease(KeyEvent.VK_C);
      robot.keyRelease(KeyEvent.VK_CONTROL);

      try {
        //sleep just a little to let the clipboard contents get updated
        Thread.sleep(25);
      } catch (InterruptedException e) {
          e.printStackTrace();
      }

      Transferable t = Toolkit.getDefaultToolkit().getSystemClipboard().getContents(null);
      try
      {
          if (t != null && t.isDataFlavorSupported(DataFlavor.stringFlavor))
          {
              String text = (String) t.getTransferData(DataFlavor.stringFlavor);
              System.out.println(text);
          }
      }
      catch (Exception ex)
      {
          ex.printStackTrace();
      }
      
      
      if (true)
      System.exit(1);
      
      JFrame jf = new JFrame();
      jf.setSize(100, 100);
      jf.setLocationRelativeTo(null);
      jf.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      
      JLabel jl = new JLabel("asdf");
      jl.setSize(jf.getSize());
      jl.addKeyListener(new KeyListener() {
        
        public void keyTyped(KeyEvent e) {
          
        }
        
        public void keyReleased(KeyEvent e) {
          
          
        }
        
        public void keyPressed(KeyEvent e) {

          System.out.println(e.getKeyCode());
          System.out.println(KeyEvent.VK_CONTROL + " . " + KeyEvent.VK_C);
          
        }
      });
      jf.add(jl);
      jf.setVisible(true);

      jl.requestFocus();
    }
}