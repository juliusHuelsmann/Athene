package spotlight.delphi.control;

import java.awt.Point;


/**
 * This interface is to be implemented by a class defined in the main project
 * Spotlight-Zeus.
 * 
 * @author Julius Huelsmann
 * @version %I%, %U$
 * @since 1.0
 */
public interface CaptureListener {

  
  /**
   * Is called in case a point is recorded that is to be used for extracting
   * the word next to the point.
   * 
   * @param p the point at which the word is approximately located.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   */
  abstract void pointCaptured(final Point p);
  
  /**
   * Is called in case a word is directly captured.
   * 
   * @param s the word that is to be looked-up inside the specified 
   *          dictionaries.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   */
  abstract void wordCaptured(final String s);
}
