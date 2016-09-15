package spotlight.delphi.model;

import java.io.Serializable;


/**
 * The settings class for the Delphi project that copes with capturing the 
 * location of a word to be looked-up or even the word (according to the 
 * settings specified inside this class).
 * 
 * @author Julius Huelsmann
 * @version %I%, %U%
 * @since 1.0
 */
public class SettingsDelphi implements Serializable {
  
  
  /**
   * Default serial version UID for being able to store instances of this class
   * to the hard drive.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   */
  private static final long serialVersionUID = 1L;


  /**
   * Whether the point of the click is to be captured in case the user clicks
   * for a predefined amount of seconds (hard coded by now, visual feedback
   * is given to the user that indicates the amount of time to press).
   * 
   * <p>
   * In case this value is true, an opaque overlay window is created that
   * forwards the click and scroll events to the underlying windows and
   * calls the matching function inside the CaputreListener implementation.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   */
  private boolean longClick;
  

  /**
   * Whether the word is to be captured in case the user selects a string and
   * presses the shift button.
   * 
   * <p>
   * In case this value is true, an opaque overlay window is created that
   * forwards the click and scroll events to the underlying windows and
   * calls the matching function inside the CaputreListener implementation.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   */
  private boolean shiftDown;

  
  /**
   * Constructor: initializes the {@link #shiftDown} and {@link #longClick} 
   * with their default values.
   */
  public SettingsDelphi() {
    this.longClick = false;
    this.shiftDown = true;
  }

  /**
   * Returns whether {@link #longClick}.
   * Whether the point of the click is to be captured in case the user clicks
   * for a predefined amount of seconds (hard coded by now, visual feedback
   * is given to the user that indicates the amount of time to press).
   * 
   * <p>
   * In case this value is true, an opaque overlay window is created that
   * forwards the click and scroll events to the underlying windows and
   * calls the matching function inside the CaputreListener implementation.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   * @return the longClick
   */
  public final boolean isLongClick() {
    return longClick;
  }


  /**
   * Sets the {@link #longClick}.
   * Whether the point of the click is to be captured in case the user clicks
   * for a predefined amount of seconds (hard coded by now, visual feedback
   * is given to the user that indicates the amount of time to press).
   * 
   * <p>
   * In case this value is true, an opaque overlay window is created that
   * forwards the click and scroll events to the underlying windows and
   * calls the matching function inside the CaputreListener implementation.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   * @param xlongClick the longClick to set
   */
  public final void setLongClick(final boolean xlongClick) {
    this.longClick = xlongClick;
  }


  /**
   * Returns the {@link #shiftDown}.
   * Whether the word is to be captured in case the user selects a string and
   * presses the shift button.
   * 
   * <p>
   * In case this value is true, an opaque overlay window is created that
   * forwards the click and scroll events to the underlying windows and
   * calls the matching function inside the CaputreListener implementation.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   * @return the shiftDown
   */
  public final boolean isShiftDown() {
    return shiftDown;
  }


  /**
   * Sets the {@link #shiftDown}.
   * Whether the word is to be captured in case the user selects a string and
   * presses the shift button.
   * 
   * <p>
   * In case this value is true, an opaque overlay window is created that
   * forwards the click and scroll events to the underlying windows and
   * calls the matching function inside the CaputreListener implementation.
   * 
   * @author Julius Huelsmann
   * @since 1.0
   * @param xshiftDown the shiftDown to set
   */
  public final void setShiftDown(final boolean xshiftDown) {
    this.shiftDown = xshiftDown;
  }
}
