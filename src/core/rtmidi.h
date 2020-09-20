/**********************************************************************/
/*! \class RtMidi
    \brief An abstract base class for realtime MIDI input/output.

    This class implements some common functionality for the realtime
    MIDI input/output subclasses RtMidiIn and RtMidiOut.

    RtMidi WWW site: http://music.mcgill.ca/~gary/rtmidi/

    RtMidi: realtime MIDI i/o C++ classes
    Copyright (c) 2003-2005 Gary P. Scavone

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    Any person wishing to distribute modifications to the Software is
    requested to send the modifications to the original developer so that
    they can be incorporated into the canonical version.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**********************************************************************/

/************************************************************************/
/*! \class RtMidiError
 \brief Exception handling class for RtAudio & RtMidi.
 
 The RtMidiError class is quite simple but it does allow errors to be
 "caught" by RtMidiError::Type. See the RtAudio and RtMidi
 documentation to know which methods can throw an RtMidiError.
 
 */
/************************************************************************/

#ifndef RT_MIDI_ERROR_H // REFACTOR-2017: new abomination!
#define RT_MIDI_ERROR_H

#include <exception>
#include <iostream>
#include <string>

class RtMidiError : public std::exception
{
public:
    //! Defined RtMidiError types.
    enum Type {
        WARNING,           /*!< A non-critical error. */
        DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
        UNSPECIFIED,       /*!< The default, unspecified error type. */
        NO_DEVICES_FOUND,  /*!< No devices found on system. */
        INVALID_DEVICE,    /*!< An invalid device ID was specified. */
        MEMORY_ERROR,      /*!< An error occured during memory allocation. */
        INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
        INVALID_USE,       /*!< The function was called incorrectly. */
        DRIVER_ERROR,      /*!< A system driver error occured. */
        SYSTEM_ERROR,      /*!< A system error occured. */
        THREAD_ERROR       /*!< A thread error occured. */
    };
    
    //! The constructor.
    RtMidiError( const std::string& message, Type type = RtMidiError::UNSPECIFIED )
        throw() : message_(message), type_(type) {}
    
    //! The destructor.
    virtual ~RtMidiError( void ) throw() {}
    
    //! Prints thrown error message to stderr.
    virtual void printMessage( void ) const throw() { std::cerr << '\n' << message_ << "\n\n"; }

    //! Returns the thrown error message type.
    virtual const Type& getType(void) const throw() { return type_; }
    
    //! Returns the thrown error message string.
    virtual const std::string& getMessage(void) const throw() { return message_; }
    
    //! Returns the thrown error message as a c-style string.
    virtual const char* what( void ) const throw() { return message_.c_str(); }
        
protected:
    std::string message_;
    Type type_;
};

#endif



// RtMidi: Version 1.0.4, 14 October 2005

#ifndef RTMIDI_H
#define RTMIDI_H

#include <string>

class RtMidi
{
 public:

  //! Pure virtual openPort() function.
  virtual void openPort( unsigned int portNumber = 0 ) = 0;

  //! Pure virtual openVirtualPort() function.
  virtual void openVirtualPort() = 0;

  //! Pure virtual getPortCount() function.
  virtual unsigned int getPortCount() = 0;

  //! Pure virtual getPortName() function.
  virtual std::string getPortName( unsigned int portNumber = 0 ) = 0;

  //! Pure virtual closePort() function.
  virtual void closePort( void ) = 0;

 protected:

  RtMidi();
  virtual ~RtMidi() {};

  // A basic error reporting function for internal use in the RtMidi
  // subclasses.  The behavior of this function can be modified to
  // suit specific needs.
  void error( RtMidiError::Type type );

  void *apiData_;
  bool connected_;
  std::string errorString_;
};




/**********************************************************************/
/*! \class RtMidiIn
    \brief A realtime MIDI input class.

    This class provides a common, platform-independent API for
    realtime MIDI input.  It allows access to a single MIDI input
    port.  Incoming MIDI messages are either saved to a queue for
    retrieval using the getMessage() function or immediately passed to
    a user-specified callback function.  Create multiple instances of
    this class to connect to more than one MIDI device at the same
    time.  With the OS-X and Linux ALSA MIDI APIs, it is also possible
    to open a virtual input port to which other MIDI software clients
    can connect.

    by Gary P. Scavone, 2003-2004.
*/
/**********************************************************************/

#include <vector>
#include <queue>

class RtMidiIn : public RtMidi
{
 public:

  //! User callback function type definition.
  typedef void (*RtMidiCallback)( double timeStamp, std::vector<unsigned char> *message, void *userData);

  //! Default constructor.
  /*!
      An exception will be thrown if a MIDI system initialization error occurs.
  */
  RtMidiIn();

  //! If a MIDI connection is still open, it will be closed by the destructor.
  ~RtMidiIn();

  //! Open a MIDI input connection.
  /*!
      An optional port number greater than 0 can be specified.
      Otherwise, the default or first port found is opened.
  */
  void openPort( unsigned int portNumber = 0 );

  //! Create a virtual input port to allow software connections (OS X and ALSA only).
  /*!
      This function creates a virtual MIDI input port to which other
      software applications can connect.  This type of functionality
      is currently only supported by the Macintosh OS-X and Linux ALSA
      APIs (the function does nothing for the other APIs).
  */
  void openVirtualPort();

  //! Set a callback function to be invoked for incoming MIDI messages.
  /*!
      The callback function will be called whenever an incoming MIDI
      message is received.  While not absolutely necessary, it is best
      to set the callback function before opening a MIDI port to avoid
      leaving some messages in the queue.
  */
  void setCallback( RtMidiCallback callback, void *userData = 0 );

  //! Cancel use of the current callback function (if one exists).
  /*!
      Subsequent incoming MIDI messages will be written to the queue
      and can be retrieved with the \e getMessage function.
  */
  void cancelCallback();

  //! Close an open MIDI connection (if one exists).
  void closePort( void );

  //! Return the number of available MIDI input ports.
  unsigned int getPortCount();

  //! Return a string identifier for the specified MIDI input port number.
  /*!
      An exception is thrown if an invalid port specifier is provided.
  */
  std::string getPortName( unsigned int portNumber = 0 );

  //! Set the maximum number of MIDI messages to be saved in the queue.
  /*!
      If the queue size limit is reached, incoming messages will be
      ignored.  The default limit is 1024.
  */
  void setQueueSizeLimit( unsigned int queueSize );

  //! Specify whether certain MIDI message types should be queued or ignored during input.
  /*!
      By default, active sensing messages are ignored
      during message input because of their relative high data rates.
      MIDI sysex messages are ignored by default as well.  Variable
      values of "true" imply that the respective message type will be
      ignored.
  */
  void ignoreTypes( bool midiSysex = true, bool midiTime = false, bool midiSense = true );

  //! Fill the user-provided vector with the data bytes for the next available MIDI message in the input queue and return the event delta-time in seconds.
  /*!
      This function returns immediately whether a new message is
      available or not.  A valid message is indicated by a non-zero
      vector size.  An exception is thrown if an error occurs during
      message retrieval or an input connection was not previously
      established.
  */
  double getMessage( std::vector<unsigned char> *message );

  // A MIDI structure used internally by the class to store incoming
  // messages.  Each message represents one and only one MIDI message.
  struct MidiMessage { 
    std::vector<unsigned char> bytes; 
    double timeStamp;

    // Default constructor.
    MidiMessage()
      :bytes(3), timeStamp(0.0) {}
  };

  // The RtMidiInData structure is used to pass private class data to
  // the MIDI input handling function or thread.
  struct RtMidiInData {
    std::queue<MidiMessage> queue;
    unsigned int queueLimit;
    unsigned char ignoreFlags;
    bool doInput;
    bool firstMessage;
    void *apiData;
    bool usingCallback;
    void *userCallback;
    void *userData;

    // Default constructor.
    RtMidiInData()
      : queueLimit(1024), ignoreFlags(5), doInput(false), firstMessage(true),
        apiData(0), usingCallback(false), userCallback(0), userData(0) {}
  };

 private:

  void initialize( void );
  RtMidiInData inputData_;

};

/**********************************************************************/
/*! \class RtMidiOut
    \brief A realtime MIDI output class.

    This class provides a common, platform-independent API for MIDI
    output.  It allows one to probe available MIDI output ports, to
    connect to one such port, and to send MIDI bytes immediately over
    the connection.  Create multiple instances of this class to
    connect to more than one MIDI device at the same time.

    by Gary P. Scavone, 2003-2004.
*/
/**********************************************************************/

class RtMidiOut : public RtMidi
{
 public:

  //! Default constructor.
  /*!
      An exception will be thrown if a MIDI system initialization error occurs.
  */
  RtMidiOut();

  //! The destructor closes any open MIDI connections.
  ~RtMidiOut();

  //! Open a MIDI output connection.
  /*!
      An optional port number greater than 0 can be specified.
      Otherwise, the default or first port found is opened.  An
      exception is thrown if an error occurs while attempting to make
      the port connection.
  */
  void openPort( unsigned int portNumber = 0 );

  //! Close an open MIDI connection (if one exists).
  void closePort();

  //! Create a virtual output port to allow software connections (OS X and ALSA only).
  /*!
      This function creates a virtual MIDI output port to which other
      software applications can connect.  This type of functionality
      is currently only supported by the Macintosh OS-X and Linux ALSA
      APIs (the function does nothing with the other APIs).  An
      exception is thrown if an error occurs while attempting to create
      the virtual port.
  */
  void openVirtualPort();

  //! Return the number of available MIDI output ports.
  unsigned int getPortCount();

  //! Return a string identifier for the specified MIDI port type and number.
  /*!
      An exception is thrown if an invalid port specifier is provided.
  */
  std::string getPortName( unsigned int portNumber );

  //! Immediately send a single message out an open MIDI output port.
  /*!
      An exception is thrown if an error occurs during output or an
      output connection was not previously established.
  */
  void sendMessage( std::vector<unsigned char> *message );

 private:

  void initialize( void );
};

#endif
