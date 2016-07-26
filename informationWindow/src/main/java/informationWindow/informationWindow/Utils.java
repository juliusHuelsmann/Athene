package informationWindow.informationWindow;

import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.IOException;

import javax.imageio.ImageIO;


public class Utils {

  /**
       * Read an image from file and resize it afterwards.
       * 
       * @param _width  The new width of the image.
       * 
       * @param _height The new height of the image.
       * 
       * @return      The resized BufferedImage.
       */
      public static BufferedImage resize(
          final String _path, final int _width, final int _height) { 
       
        BufferedImage img_scaled;
      try {
            img_scaled = ImageIO.read(AnimationWindow.class.getResourceAsStream(_path));
        //img_scaled = ImageIO.read(new File(myPath));
          return resize(img_scaled, _width, _height);
      } catch (IOException e) {
  
  
        System.out.println(_path);
        e.printStackTrace();
        return null;
      }
    }
      /**
       * Function for resizing a bufferedImage.
       * 
       * @param _bi     the bufferedImage that is to be resized
       * 
       * @param _width  its new width 
       * 
       * @param _height   its new height
       * 
       * @return      the resized BufferedImage.
       */
      public static BufferedImage resize(
          final BufferedImage _bi, final int _width, final int _height) { 
      
        
        Image img_scaled = _bi.getScaledInstance(
            _width, 
            _height, 
            Image.SCALE_SMOOTH);
        
          BufferedImage bi = new BufferedImage(
              _width, 
              _height, 
              BufferedImage.TYPE_INT_ARGB);
      
          Graphics2D
          g2d = bi.createGraphics();
          g2d.drawImage(img_scaled, 0, 0, null);
          g2d.dispose();
      
          return bi;
      }

}
