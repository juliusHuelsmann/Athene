package informationWindow.informationWindow;

import java.awt.Color;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Window;
import java.awt.image.BufferedImage;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JWindow;
import javax.swing.border.EmptyBorder;

public class AnimationWindow extends JFrame  {
  

  private BufferedImage bi_animation;
  private Thread tr;
  private JLabel jlbl_animation;
  int origwidth;
  int origheight;
  /**
   * 
   */
  public AnimationWindow() {
    super.setLayout(null);
    origwidth = 100;
    origheight = 100;
    super.setSize(origwidth, origheight);
    super.setVisible(false);
    
    super.setDefaultLookAndFeelDecorated(false);
    super.setOpacity(1);
    setUndecorated(true);
    

    initAnimation();
    jlbl_animation = new JLabel();
    jlbl_animation.setSize(getSize());
    jlbl_animation.setLocation(0, 0);
    super.add(jlbl_animation);
  }
  
  private void initAnimation() {

    bi_animation = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_ARGB);
//    for (int i = 0; i < bi_animation.getWidth(); i++) {
//      for (int j = 0; j < bi_animation.getHeight(); j++) {
//        bi_animation.setRGB(i, j, new Color(0, 0, 0, 0).getRGB());
//      }
//    }    
}

  
  public void drawAnimation() {
    if (tr == null) {
      tr = new Thread() {
        public void run() {
          final int rgb = new Color(0, 0, 0, 255).getRGB();
          
          int prevx = -1;
          int prevy = -1;
          double unterteilung = 0.1;
          for (double i = 0; i <= 2 * Math.PI; i= 1.0 * i + unterteilung) {
            
            int xcoord = (int) (getWidth()/2 + Math.sin(i) * getWidth()/2);
            int ycoord = (int) (getWidth()/2 + Math.cos(i) * getWidth()/2);
            
            Graphics g = bi_animation.getGraphics();
            
            final int maxval = 7;
            for (int j = 0; j < maxval && prevx != -1; j++) {
              int newxval = (int) Math.round(prevx + 1.0 * j / maxval * (xcoord- prevx));
              int newyval = (int) Math.round(prevy + 1.0 * j/ maxval * (ycoord- prevy));
              if (newxval >= 0 && newyval >= 0 && newxval < getWidth() && newyval < getHeight()) {

                bi_animation.setRGB(
                    newxval, 
                    newyval,
                    rgb);
              }
            }

            jlbl_animation.setIcon(new ImageIcon(bi_animation));
            jlbl_animation.repaint();
            try {
              Thread.sleep((long) (mindif / (2.0 * Math.PI / unterteilung)));
            } catch (InterruptedException e) {
              interrupt();
              tr = null;
            }
            
            prevx = xcoord;
            prevy = ycoord;
          }
          tr = null;
        }
      };
      tr.run();
    }
  }

  
  public void setVisible(final boolean b) {
    super.setVisible(b);
    if (b) {
      new Thread() {
        public void run() {

          drawAnimation();
        }
      } .start();
    }
  }
  private long mindif;
  public void prepare(
      final int imgx, final int imgy, 
      int i, int j, final long mindifference,  BufferedImage bi) {
    super.setLocation(i, j);
    this.mindif = mindifference;
    int imgxx = imgx;
    int imgxy = imgy;
    if (i <= 0) {
      setSize(origwidth + i, getHeight());
      imgxx += i;
      i =0;
    } else {
      setSize(origwidth, getHeight());
    }
    if (j <= 0) {
      setSize(getWidth(), origheight + j);
      imgxy += j;
      i =0;
    } else {
      setSize(getWidth(), origheight);
    }
    bi_animation = bi.getSubimage(imgxx, imgxy, getWidth(), getHeight());
    jlbl_animation.setIcon(new ImageIcon(bi_animation));
  }
  
  
}
