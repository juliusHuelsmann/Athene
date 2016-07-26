package informationWindow.informationWindow.utils;

import java.awt.image.BufferedImage;

public class ImageTupel {
  private final BufferedImage i; private final int index;
  public ImageTupel(final BufferedImage i, final int index) {
    this.i = i;
    this.index = index;
  }
  /**
   * @return the i
   */
  public BufferedImage getI() {
    return i;
  }
  /**
   * @return the index
   */
  public int getIndex() {
    return index;
  }
}
