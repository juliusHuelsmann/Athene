package informationWindow.informationWindow;

import java.awt.Color;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;



/**
 * 
 * JFrame that should not be visible at all but show a little animation that
 * is showing the amount of time the user has to wait for the click to become
 * a lookup-click. (This time is take for being able to distinguish between
 * a "real-"click that is just forwarded to the underlying window and a 
 * "lookup-" click)
 * 
 * @author Julius Huelsmann
 * @version %I%, %U%
 * @since 0.0
 */
@SuppressWarnings("serial")
public class AnimationWindow extends JFrame  {
  

  
  /**
   * BufferedImage which contains the extracted segment of the screenshot that 
   * is overlaid by the animation.
   */
  private BufferedImage bi_animation;
  
  /**
   * The source image that contains the rough enhanced animation that is
   * taken and drawn on top of the {@link #bi_animation}.
   */
  private BufferedImage bi_enhancedAnimationSource;
  
  /**
   * Thread that 
   */
  private Thread tr;
  private JLabel jlbl_animation;
  
  
  
  /**
   * The original width of the JFrame. May change in case some area is selected
   * that is not entirely part of the displayed screen.
   * In that case the size might be adapted but restored the next time.
   */
  private final int origwidth;
  
  /**
   * The original height of the JFrame. May change in case some area is selected
   * that is not entirely part of the displayed screen.
   * In that case the size might be adapted but restored the next time.
   */
  private final int origheight;
  
  
  /**
   * Whether to execute {@link #drawEnhancedAnimation()} (true) or 
   * {@link #drawSimpleAnimation()} (false).
   */
  private boolean enhancedEnabled = true;
  
  /**
   * The source image of {@link #bi_enhancedAnimationSource}.
   */
  private final String enhancedAnimationInputSource = "/informationWindow/informationWindow/res/ani1.png";
  
  /**
   * 
   */
  public AnimationWindow() {

    // initialize the original width and height
    origwidth = 100;
    origheight = 100;
    
    
    // adapt the settings of the Window
    super.setLayout(null);
    super.setSize(origwidth, origheight);
    super.setLocation(-getWidth(), -getHeight());
    super.setDefaultLookAndFeelDecorated(false);
    super.setOpacity(1);
    super.setUndecorated(true);
    
    // initialize the animation BufferedImage.
    bi_animation = new BufferedImage(getWidth(), 
        getHeight(), BufferedImage.TYPE_INT_ARGB);
    
    if (enhancedEnabled) {
      bi_enhancedAnimationSource = Utils.resize(enhancedAnimationInputSource, getWidth(), getHeight());
    }
    
    // initialize the animation JLabel.
    jlbl_animation = new JLabel();
    jlbl_animation.setSize(getSize());
    jlbl_animation.setLocation(0, 0);
    super.add(jlbl_animation);
    super.setVisible(true);
  }

  
  public void drawAnimation() {
    if (enhancedEnabled) {
      drawEnhancedAnimation();
    } else {
      drawSimpleAnimation();
    }
    
  }
  /**
   * Draw an animation.
   */
  private void drawEnhancedAnimation() {
    if (tr == null) {
      tr = new Thread() {
        public void run() {

          final int rgbWhite = new Color(255, 255, 255).getRGB();
          
          final Point origin = new Point(getWidth() / 2, getHeight() / 2);
          Point oldBorder = new Point(-1, -1);
          double unterteilung = 0.01;
          for (double i = 0; i <= 2 * Math.PI; i= 1.0 * i + unterteilung) {
            
            Point newBorder = new Point(
                (int) (origin.x + Math.sin(Math.PI - i) * getWidth()/2),
                (int) (origin.y + Math.cos(Math.PI - i) * getHeight()/2));
            
            
            // if the old border has already been initialized
            if (oldBorder.x != -1) {

              final double angleOld = Math.atan2(oldBorder.y,  oldBorder.x);
              final double angleNew = Math.atan2(newBorder.y,  newBorder.x);

              final double maxAngle = Math.max(angleOld, angleNew);
              final double minAngle = Math.min(angleOld, angleNew);

              final int minCol = Math.min(oldBorder.x, 
                  Math.min(newBorder.x, origin.x));
              final int maxCol = Math.max(oldBorder.x,
                  Math.max(newBorder.x, origin.x));
              final int minRow = Math.min(oldBorder.y, 
                  Math.min(newBorder.y, origin.y));
              final int maxRow = Math.max(oldBorder.y, 
                  Math.max(newBorder.y, origin.y));
              
              for (int c = minCol; c <= maxCol + getWidth() / 10; c++) {
                for (int r = minRow; r <= maxRow + getHeight() / 10; r++) {
                  
                  if (c < 0 || r < 0 
                      || r >= bi_enhancedAnimationSource.getHeight() 
                      || c >= bi_enhancedAnimationSource.getWidth()) {
                    continue;
                  }
                  
                  final double angle = Math.atan2(r, c);
                  if (0.1 < minAngle - angle) {
                    continue;
                  } else if (angle - maxAngle> 0.1) {
                    continue; // not really possible to differentiate and make it quicker...
                  } else {
                    
                    // Draw pixel to image
                    final int rgbCurrent = bi_enhancedAnimationSource.getRGB(c, r);
                    if (rgbCurrent == rgbWhite) {
                      continue;
                    } else {

                      final Color col = new Color(rgbCurrent);
                      final Color colT = new Color(bi_animation.getRGB(c, r));
                      final double percR = 255.0 / (255 - col.getRed());
                      final double percG = 255.0 / (255 - col.getGreen());
                      final double percB = 255.0 / (255 - col.getBlue());
                      
                      final double red = Math.min(255, 
                          col.getRed() * percR + (1 - percR) * colT.getRed());
                      final double green = Math.min(255, 
                          col.getGreen() * percG + (1 - percG) * colT.getGreen());
                      final double blue = Math.min(255, 
                          col.getBlue() * percB + (1 - percB) * colT.getBlue());
                      Color col2use = new Color(
                          (int) (red),
                          (int) (green),
                          (int) (blue));
                      bi_animation.setRGB(c, r, col2use.getRGB());
                    }
                  }
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
              
            }
            
            oldBorder = newBorder;
          }
          tr = null;
        }
      };
      tr.run();
    }
  }


  
  
  /**
   * Draw an animation.
   */
  private void drawSimpleAnimation() {
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
      int imgx, int imgy, 
      int i, int j, final long mindifference,  BufferedImage bi) {

    final int minx = Math.min(imgx, i);
    final int maxx = Math.max(imgx, i);
    final int miny = Math.min(imgy, j);
    final int maxy = Math.max(imgy, j);
    
    int width = origwidth;
    int height = origheight;

    if (minx < 0) {
      imgx -= minx;
      i -= minx;
      width += minx;
    } 
    if (maxx + width > bi.getWidth()) {
      final int difference = maxx + width - bi.getWidth();
      width -= difference;
    }
    if (miny < 0) {
      imgy -= miny;
      j -= miny;
      height += miny;
    } 
    if (maxy + height > bi.getHeight()) {
      final int difference = maxy + height - bi.getHeight();
      height -= difference;
    }
    
    super.setLocation(i, j);
    super.setSize(width, height);
    
    this.mindif = mindifference;
    
    bi_animation = bi.getSubimage(imgx, imgy, getWidth(), getHeight());
    
    if (getWidth() > bi_enhancedAnimationSource.getWidth()
        || getHeight() > bi_enhancedAnimationSource.getHeight()) {
      // neu laden

      if (enhancedEnabled) {
        bi_enhancedAnimationSource = Utils.resize(enhancedAnimationInputSource, 
            origwidth, origheight);
      }
    } 
    
    if (getWidth() != bi_enhancedAnimationSource.getWidth()
        || getHeight() != bi_enhancedAnimationSource.getHeight()) {
      if (enhancedEnabled) {

        bi_enhancedAnimationSource = bi_enhancedAnimationSource.getSubimage(
            origwidth - getWidth(), origheight - getHeight(),
            getWidth(), getHeight());
      }
    }
    jlbl_animation.setIcon(new ImageIcon(bi_animation));
  }
  /**
   * {@inheritDoc}
   * <p>
   * The {@code width} and {@code height} values
   * are automatically enlarged if either is less than
   * the minimum size as specified by previous call to
   * {@code setMinimumSize}.
   * <p>
   * The method changes the geometry-related data. Therefore,
   * the native windowing system may ignore such requests, or it may modify
   * the requested data, so that the {@code Window} object is placed and sized
   * in a way that corresponds closely to the desktop settings.
   *
   * @see #getSize
   * @see #setBounds
   * @see #setMinimumSize
   * @since 1.6
   */
  @Override
  public void setSize(int width, int height) {
      if (jlbl_animation != null) {
        jlbl_animation.setSize(width, height);
      }
      super.setSize(width, height);
  }
  
}
