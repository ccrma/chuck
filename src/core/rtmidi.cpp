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

// RtMidi: Version 1.0.4, 14 October 2005

// chuck
#include "rtmidi.h"
#include <sstream>
#include <iostream>
#include <stdio.h>


//*********************************************************************//
//  Common RtMidi Definitions
//*********************************************************************//

RtMidi :: RtMidi()
  : apiData_( 0 ), connected_( false )
{
}

// chuck
void RtMidi :: error( RtMidiError::Type type )
{
  if (type == RtMidiError::WARNING) {
    std::cerr << "[chuck](via RtMidi): " << errorString_ << "\n";
  }
  else if (type == RtMidiError::DEBUG_WARNING) {
#if defined(__RTMIDI_DEBUG__)
    std::cerr << "[chuck](via RtMidi): " << errorString_ << "\n";
#endif
  }
  else {
    // std::cerr << "[chuck](via RtMidi): " << errorString_ << "\n";
    throw RtMidiError( errorString_, type );
  }
}


//*********************************************************************//
//  Common RtMidiIn Definitions
//*********************************************************************//

RtMidiIn :: RtMidiIn() : RtMidi()
{
  this->initialize();
}

void RtMidiIn :: setCallback( RtMidiCallback callback, void *userData )
{
  if ( inputData_.usingCallback ) {
    errorString_ = "RtMidiIn::setCallback: callback function is already set!";
    error( RtMidiError::WARNING );
    return;
  }

  if ( !callback ) {
    errorString_ = "RtMidiIn::setCallback: callback function value is invalid!";
    error( RtMidiError::WARNING );
    return;
  }

  inputData_.userCallback = (void *) callback;
  inputData_.userData = userData;
  inputData_.usingCallback = true;
}

void RtMidiIn :: cancelCallback()
{
  if ( !inputData_.usingCallback ) {
    errorString_ = "RtMidiIn::cancelCallback: no callback function was set!";
    error( RtMidiError::WARNING );
    return;
  }

  inputData_.userCallback = 0;
  inputData_.userData = 0;
  inputData_.usingCallback = false;
}

void RtMidiIn :: setQueueSizeLimit( unsigned int queueSize )
{
  inputData_.queueLimit = queueSize;
}

void RtMidiIn :: ignoreTypes( bool midiSysex, bool midiTime, bool midiSense )
{
  inputData_.ignoreFlags = 0;
  if ( midiSysex ) inputData_.ignoreFlags = 0x01;
  if ( midiTime ) inputData_.ignoreFlags |= 0x02;
  if ( midiSense ) inputData_.ignoreFlags |= 0x04;
}

double RtMidiIn :: getMessage( std::vector<unsigned char> *message )
{
  message->clear();

  if ( inputData_.usingCallback ) {
    errorString_ = "RtMidiIn::getNextMessage: user callback is currently set for this port.";
    error( RtMidiError::WARNING );
    return 0.0;
  }

  if ( inputData_.queue.size() == 0 ) return 0.0;

  // Copy queued message to the vector pointer argument and then "pop" it.
  std::vector<unsigned char> *bytes = &(inputData_.queue.front().bytes);
  message->assign( bytes->begin(), bytes->end() );
  double deltaTime = inputData_.queue.front().timeStamp;
  inputData_.queue.pop();

  return deltaTime;
}

//*********************************************************************//
//  Common RtMidiOut Definitions
//*********************************************************************//

RtMidiOut :: RtMidiOut() : RtMidi()
{
  this->initialize();
}


//*********************************************************************//
//  API: Macintosh OS-X
//*********************************************************************//

// API information found at:
//   - http://developer. apple .com/audio/pdf/coreaudio.pdf 

#if defined(__MACOSX_CORE__)

// The CoreMIDI API is based on the use of a callback function for
// MIDI input.  We convert the system specific time stamps to delta
// time values.

// OS-X CoreMIDI header files.
#include <CoreMIDI/CoreMIDI.h>
#include <CoreAudio/HostTime.h>

// CoreMIDI naming helper function prototypes
static void readable_name(MIDIEndpointRef end, char *buffer, int bufsize);
static int get_device_name(SInt32 uniqueid, char*buffer, int bufsize);

// A structure to hold variables related to the CoreMIDI API
// implementation.
struct CoreMidiData {
  MIDIClientRef client;
  MIDIPortRef port;
  MIDIEndpointRef endpoint;
  MIDIEndpointRef destinationId;
  unsigned long long lastTime;
};

// ******
// API: OS-X
// helper functions
// *****

// coreMIDI port naming helper function
/*
  This wraps up the code to take a passed-in endpoint and work out
  a nice human-readable name for it.

  For the moment, this function will return firstly the device name +
  port name, or if we can work out what external devices are hooked
  in, that name. It only grabs the very first connected device name.

  NB. won't get connected devices correctly before 10.3.
*/
static void readable_name(MIDIEndpointRef end, char *buffer, int bufsize)
{  
    MIDIEntityRef ent = 0; // NULL;
    MIDIDeviceRef dev = 0; //NULL;
    int ii, count, length;//, ret;
    SInt32 *idarray;
    CFDataRef data = NULL;
    CFStringRef s;

    buffer[0] = '\0';
  
    if( MIDIObjectGetDataProperty(end, kMIDIPropertyConnectionUniqueID, &data) == 0)
    {
        length = CFDataGetLength(data) / sizeof(SInt32);
        idarray = (SInt32 *) CFDataGetBytePtr(data);
        count = 0;
        for (ii = 0; ii < length; ii++) {
            if (bufsize < 3)
                break;
            if (count > 0) {
                strcpy(buffer, ", ");
                buffer += 2;
                bufsize -= 2;
            }
      
            if (get_device_name(idarray[ii], buffer, bufsize) == 0) {
                count++;
                bufsize -= strlen(buffer);
                buffer += strlen(buffer);
            }
        }

        CFRelease(data);
        if (count > 0)
            return;
    }

    char * mid = buffer;
    // build up the name of the enclosing device, if it can be found.
    if (MIDIEndpointGetEntity(end, &ent) == 0) {
        if (MIDIEntityGetDevice(ent, &dev) == 0) {
            if (MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &s) == 0) {
                CFStringGetCString(s, buffer, bufsize, 0);
                bufsize -= strlen(buffer) + 1;
                buffer += strlen(buffer);

                CFRelease(s);
            }
        }
    }

    // ge
    char buffy[128];
    // Now add the port/endpoint name.
    // no need to update buffer pointer. Last item.
    if (MIDIObjectGetStringProperty(end, kMIDIPropertyName, &s) == 0) {
        CFStringGetCString(s, buffy, 128, 0);
        CFRelease(s);
        
        // copy if different
        if( strcmp( mid, buffy ) && strlen(buffy) < bufsize ) {
            *buffer = ' ';
            buffer++;
            strcpy( buffer, buffy );
        }
    }
}


// port naming helper function (External devices)
/*
  Uses the midiojectfindbyuniqueid function to hunt down the relevant
  device and copies its name into the buffer provided. We don't know
  what type of device we'll get back, so we first throw away any
  external flags (We don't care), and then cast it up to device which
  is the thing which has a useful name.

  If it can't find one, return -1. 0 returned on success.
*/
static int get_device_name(SInt32 uniqueid, char *buffer, int bufsize)
{
    int ret;
    MIDIObjectRef object = 0; // NULL; // 1.3.1.0
    MIDIObjectType type;

    MIDIDeviceRef dev = 0; // NULL;
    MIDIEntityRef ent = 0; // NULL;
    MIDIEndpointRef end = 0; // NULL;
    CFStringRef name = NULL;

    ret = MIDIObjectFindByUniqueID(uniqueid, &object, &type);
    if (ret < 0)
        return -1;

    // now clear any external flag.
    if (type > 0) 
        type = (MIDIObjectType) (type & (~kMIDIObjectType_ExternalMask));

    if (type == kMIDIObjectType_Device) {
        dev = (MIDIDeviceRef) object;
    } else  if (type == kMIDIObjectType_Entity) {
        ent = (MIDIEntityRef) object;
        if (MIDIEntityGetDevice(ent, &dev)) {
            return -1;
        }
    } if (type == kMIDIObjectType_Source ||
        type == kMIDIObjectType_Destination) {
        end = (MIDIEndpointRef) object;
        if (MIDIEndpointGetEntity(end, &ent)) {
            return -1;
        }
        if (MIDIEntityGetDevice(ent, &dev)) {
            return -1;
        }
    } else {
        // unknown type
        printf("Unknown type %d returned from findobject\n", (int) type);
        CFRelease( &object ); // 1.3.1.0
        return -1;
    }

    MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &name);
    CFStringGetCString(name, buffer, bufsize, 0);
    CFRelease(name);

    return 0;
}





//*********************************************************************//
//  API: OS-X
//  Class Definitions: RtMidiIn
//*********************************************************************//

void midiInputCallback( const MIDIPacketList *list, void *procRef, void *srcRef )
{
  RtMidiIn::RtMidiInData *data = static_cast<RtMidiIn::RtMidiInData *> (procRef);
  CoreMidiData *apiData = static_cast<CoreMidiData *> (data->apiData);

  bool continueSysex = false;
  unsigned char status;
  unsigned short nBytes, iByte, size;
  unsigned long long time;
  RtMidiIn::MidiMessage message;
  const MIDIPacket *packet = &list->packet[0];
  for ( unsigned int i=0; i<list->numPackets; ++i ) {

    // My interpretation of the CoreMIDI documentation: all message
    // types, except sysex, are complete within a packet and there may
    // be several of them in a single packet.  Sysex messages can be
    // broken across multiple packets but are bundled alone within a
    // packet.  I'm assuming that sysex messages, if segmented, must
    // be complete within the same MIDIPacketList.

    nBytes = packet->length;
    if ( nBytes == 0 ) continue;

    // Calculate time stamp.
    message.timeStamp = 0.0;
    if ( data->firstMessage )
      data->firstMessage = false;
    else {
      time = packet->timeStamp;
      time -= apiData->lastTime;
      time = AudioConvertHostTimeToNanos( time );
      message.timeStamp = time * 0.000000001;
    }
    apiData->lastTime = packet->timeStamp;

    iByte = 0;
    if ( continueSysex ) {
      // We have a continuing, segmented sysex message.
      if ( !(data->ignoreFlags & 0x01) ) {
        // If we're not ignoring sysex messages, copy the entire packet.
        for ( unsigned int j=0; j<nBytes; j++ )
          message.bytes.push_back( packet->data[j] );
      }
      if ( packet->data[nBytes] == 0xF7 ) continueSysex = false;
      if ( !continueSysex ) {
        // If not a continuing sysex message, invoke the user callback function or queue the message.
        if ( data->usingCallback && message.bytes.size() > 0 ) {
          RtMidiIn::RtMidiCallback callback = (RtMidiIn::RtMidiCallback) data->userCallback;
          callback( message.timeStamp, &message.bytes, data->userData );
        }
        else {
          // As long as we haven't reached our queue size limit, push the message.
          if ( data->queueLimit > data->queue.size() )
            data->queue.push( message );
          else
            std::cerr << "\nRtMidiIn: message queue limit reached!!\n\n";
        }
        message.bytes.clear();
      }
    }
    else {
      while ( iByte < nBytes ) {
        size = 0;
        // We are expecting that the next byte in the packet is a status byte.
        status = packet->data[iByte];
        if ( !(status & 0x80) ) break;
        // Determine the number of bytes in the MIDI message.
        if ( status < 0xC0 ) size = 3;
        else if ( status < 0xE0 ) size = 2;
        else if ( status < 0xF0 ) size = 3;
        else if ( status == 0xF0 ) {
          // A MIDI sysex
          if ( data->ignoreFlags & 0x01 ) {
            size = 0;
            iByte = nBytes;
          }
          else size = nBytes - iByte;
          if ( packet->data[nBytes] == 0xF7 ) continueSysex = false;
        }
        else if ( status < 0xF3 ) {
          if ( status == 0xF1 && (data->ignoreFlags & 0x02) ) {
            // A MIDI time code message and we're ignoring it.
            size = 0;
            iByte += 3;
          }
          else size = 3;
        }
        else if ( status == 0xF3 ) size = 2;
        else if ( status == 0xF8 ) {
          size = 1;
          if ( data->ignoreFlags & 0x02 ) {
            // A MIDI timing tick message and we're ignoring it.
            size = 0;
            iByte += 3;
          }
        }
        else if ( status == 0xFE && (data->ignoreFlags & 0x04) ) {
          // A MIDI active sensing message and we're ignoring it.
          size = 0;
          iByte += 1;
        }
        else size = 1;

        // Copy the MIDI data to our vector.
        if ( size ) {
          message.bytes.assign( &packet->data[iByte], &packet->data[iByte+size] );
          if ( !continueSysex ) {
            // If not a continuing sysex message, invoke the user callback function or queue the message.
            if ( data->usingCallback ) {
              RtMidiIn::RtMidiCallback callback = (RtMidiIn::RtMidiCallback) data->userCallback;
              callback( message.timeStamp, &message.bytes, data->userData );
            }
            else {
              // As long as we haven't reached our queue size limit, push the message.
              if ( data->queueLimit > data->queue.size() )
                data->queue.push( message );
              else
                std::cerr << "\nRtMidiIn: message queue limit reached!!\n\n";
            }
            message.bytes.clear();
          }
          iByte += size;
        }
      }
    }
    packet = MIDIPacketNext(packet);
  }
}

void RtMidiIn :: initialize( void )
{
  // Set up our client.
  MIDIClientRef client;
  OSStatus result = MIDIClientCreate( CFSTR("RtMidi Input Client"), NULL, NULL, &client );
  if ( result != noErr ) {
    errorString_ = "RtMidiIn::initialize: error creating OS-X MIDI client object.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Save our api-specific connection information.
  CoreMidiData *data = (CoreMidiData *) new CoreMidiData;
  data->client = client;
  data->endpoint = 0;
  apiData_ = (void *) data;
  inputData_.apiData = (void *) data;
}

void RtMidiIn :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiIn::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  unsigned int nSrc = MIDIGetNumberOfSources();
  if (nSrc < 1) {
    errorString_ = "RtMidiIn::openPort: no MIDI input sources found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

  std::ostringstream ost;
  if ( portNumber >= nSrc ) {
    ost << "RtMidiIn::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  MIDIPortRef port;
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  OSStatus result = MIDIInputPortCreate( data->client, CFSTR("RtMidi MIDI Input Port"), midiInputCallback, (void *)&inputData_, &port );
  if ( result != noErr ) {
    MIDIClientDispose( data->client );
    errorString_ = "RtMidiIn::openPort: error creating OS-X MIDI input port.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Get the desired input source identifier.
  MIDIEndpointRef endpoint = MIDIGetSource( portNumber );
  if ( endpoint == 0 ) { // 1.3.1.0: changed from NULL to 0
    MIDIPortDispose( port );
    MIDIClientDispose( data->client );
    errorString_ = "RtMidiIn::openPort: error getting MIDI input source reference.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Make the connection.
  result = MIDIPortConnectSource( port, endpoint, NULL );
  if ( result != noErr ) {
    MIDIPortDispose( port );
    MIDIClientDispose( data->client );
    errorString_ = "RtMidiIn::openPort: error connecting OS-X MIDI input port.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Save our api-specific port information.
  data->port = port;

  connected_ = true;
}

void RtMidiIn :: openVirtualPort()
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);

  // Create a virtual MIDI input destination.
  MIDIEndpointRef endpoint;
  OSStatus result = MIDIDestinationCreate( data->client, CFSTR("RtMidi Input"), midiInputCallback, (void *)&inputData_, &endpoint );
  if ( result != noErr ) {
    errorString_ = "RtMidiIn::openVirtualPort: error creating virtual OS-X MIDI destination.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Save our api-specific connection information.
  data->endpoint = endpoint;
}

void RtMidiIn :: closePort( void )
{
  if ( connected_ ) {
    CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
    MIDIPortDispose( data->port );
    connected_ = false;
  }
}

RtMidiIn :: ~RtMidiIn()
{
  // Close a connection if it exists.
  closePort();

  // Cleanup.
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  MIDIClientDispose( data->client );
  if ( data->endpoint ) MIDIEndpointDispose( data->endpoint );
  delete data;
}

unsigned int RtMidiIn :: getPortCount()
{
  return MIDIGetNumberOfSources();
}

std::string RtMidiIn :: getPortName( unsigned int portNumber )
{
//  CFStringRef nameRef;
  MIDIEndpointRef portRef;
  std::ostringstream ost;
  char name[128];

  if ( portNumber >= MIDIGetNumberOfSources() ) {
    ost << "RtMidiIn::getPortName: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }
  portRef = MIDIGetSource( portNumber );
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1030  
  readable_name(portRef, name, sizeof(name));
#else
  MIDIObjectGetStringProperty( portRef, kMIDIPropertyName, &nameRef );
  CFStringGetCString( nameRef, name, sizeof(name), 0);
  CFRelease( nameRef );
#endif
  std::string stringName = name;
  return stringName;
}

//*********************************************************************//
//  API: OS-X
//  Class Definitions: RtMidiOut
//*********************************************************************//

unsigned int RtMidiOut :: getPortCount()
{
  return MIDIGetNumberOfDestinations();
}

std::string RtMidiOut :: getPortName( unsigned int portNumber )
{
//  CFStringRef nameRef;
  MIDIEndpointRef portRef;
  std::ostringstream ost;
  char name[128];

  if ( portNumber >= MIDIGetNumberOfDestinations() ) {
    ost << "RtMidiOut::getPortName: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }
  portRef = MIDIGetDestination( portNumber );
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1030
  readable_name(portRef, name, sizeof(name));
#else
  MIDIObjectGetStringProperty( portRef, kMIDIPropertyName, &nameRef );
  CFStringGetCString( nameRef, name, sizeof(name), 0);
  CFRelease( nameRef );
#endif
  std::string stringName = name;
  return stringName;
}

void RtMidiOut :: initialize( void )
{
  // Set up our client.
  MIDIClientRef client;
  OSStatus result = MIDIClientCreate( CFSTR("RtMidi Output Client"), NULL, NULL, &client );
  if ( result != noErr ) {
    errorString_ = "RtMidiOut::initialize: error creating OS-X MIDI client object.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Save our api-specific connection information.
  CoreMidiData *data = (CoreMidiData *) new CoreMidiData;
  data->client = client;
  data->endpoint = 0;
  apiData_ = (void *) data;
}

void RtMidiOut :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiOut::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  unsigned int nDest = MIDIGetNumberOfDestinations();
  if (nDest < 1) {
    errorString_ = "RtMidiOut::openPort: no MIDI output destinations found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

  std::ostringstream ost;
  if ( portNumber >= nDest ) {
    ost << "RtMidiOut::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  MIDIPortRef port;
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  OSStatus result = MIDIOutputPortCreate( data->client, CFSTR("RtMidi Virtual MIDI Output Port"), &port );
  if ( result != noErr ) {
    MIDIClientDispose( data->client );
    errorString_ = "RtMidiOut::openPort: error creating OS-X MIDI output port.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Get the desired output port identifier.
  MIDIEndpointRef destination = MIDIGetDestination( portNumber );
  if ( destination == 0 ) { // 1.3.1.0: changed from NULL to 0
    MIDIPortDispose( port );
    MIDIClientDispose( data->client );
    errorString_ = "RtMidiOut::openPort: error getting MIDI output destination reference.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Save our api-specific connection information.
  data->port = port;
  data->destinationId = destination;
  connected_ = true;
}

void RtMidiOut :: closePort( void )
{
  if ( connected_ ) {
    CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
    MIDIPortDispose( data->port );
    connected_ = false;
  }
}

void RtMidiOut :: openVirtualPort()
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);

  if ( data->endpoint ) {
    errorString_ = "RtMidiOut::openVirtualPort: a virtual output port already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  // Create a virtual MIDI output source.
  MIDIEndpointRef endpoint;
  OSStatus result = MIDISourceCreate( data->client, CFSTR("RtMidi Output"), &endpoint );
  if ( result != noErr ) {
    errorString_ = "RtMidiOut::initialize: error creating OS-X virtual MIDI source.";
    error( RtMidiError::DRIVER_ERROR );
  }

  // Save our api-specific connection information.
  data->endpoint = endpoint;
}

RtMidiOut :: ~RtMidiOut()
{
  // Close a connection if it exists.
  closePort();

  // Cleanup.
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  MIDIClientDispose( data->client );
  if ( data->endpoint ) MIDIEndpointDispose( data->endpoint );
  delete data;
}

void RtMidiOut :: sendMessage( std::vector<unsigned char> *message )
{
  unsigned int nBytes = message->size();
  // Pad the buffer for extra (unknown) structure data.
  Byte buffer[nBytes+32];
  MIDIPacketList *pktlist = (MIDIPacketList *) buffer;
  MIDIPacket *curPacket = MIDIPacketListInit( pktlist );

  MIDITimeStamp timeStamp = 0;
  curPacket = MIDIPacketListAdd( pktlist, sizeof(buffer), curPacket, timeStamp, nBytes, &message->at(0) );

  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);

  // Send to any destinations that may have connected to us.
  OSStatus result;
  if ( data->endpoint ) {
    result = MIDIReceived( data->endpoint, pktlist );
    if ( result != noErr ) {
      errorString_ = "RtMidiOut::sendMessage: error sending MIDI to virtual destinations.";
      error( RtMidiError::WARNING );
    }
  }

  // And send to an explicit destination port if we're connected.
  if ( connected_ ) {
    result = MIDISend( data->port, data->destinationId, pktlist );
    if ( result != noErr ) {
      errorString_ = "RtMidiOut::sendMessage: error sending MIDI message to port.";
      error( RtMidiError::WARNING );
    }
  }
}

#endif  // __MACOSX_CORE__


//*********************************************************************//
//  API: LINUX ALSA SEQUENCER
//*********************************************************************//

// API information found at:
//   - http://www.alsa-project.org/documentation.php#Library

// chuck
#if defined(__LINUX_ALSASEQ__) || defined(__PLATFORM_LINUX__)

// The ALSA Sequencer API is based on the use of a callback function for
// MIDI input.
//
// Thanks to Pedro Lopez-Cabanillas for help with the ALSA sequencer
// time stamps and other assorted fixes!!!

#include <pthread.h>
#include <sys/time.h>

// ALSA header file.
#include <alsa/asoundlib.h>

// A structure to hold variables related to the ALSA API
// implementation.
struct AlsaMidiData {
  snd_seq_t *seq;
  int vport;
  snd_seq_port_subscribe_t *subscription;
  snd_midi_event_t *coder;
  unsigned int bufferSize;
  unsigned char *buffer;
  pthread_t thread;
  unsigned long long lastTime;
  int queue_id; // an input queue is needed to get timestamped events
};

#define PORT_TYPE( pinfo, bits ) ((snd_seq_port_info_get_capability(pinfo) & (bits)) == (bits))

//*********************************************************************//
//  API: LINUX ALSA
//  Class Definitions: RtMidiIn
//*********************************************************************//

extern "C" void *alsaMidiHandler( void *ptr )
{
  RtMidiIn::RtMidiInData *data = static_cast<RtMidiIn::RtMidiInData *> (ptr);
  AlsaMidiData *apiData = static_cast<AlsaMidiData *> (data->apiData);

  long nBytes;
  unsigned long long time, lastTime;
  bool continueSysex = false;
  RtMidiIn::MidiMessage message;

  snd_seq_event_t *ev;
  int result;
  apiData->bufferSize = 32;
  result = snd_midi_event_new( 0, &apiData->coder );
  if ( result < 0 ) {
    data->doInput = false;
    std::cerr << "\nRtMidiIn::alsaMidiHandler: error initializing MIDI event parser!\n\n";
    return 0;
  }
  unsigned char *buffer = (unsigned char *) malloc( apiData->bufferSize );
  if ( buffer == NULL ) {
    data->doInput = false;
    std::cerr << "\nRtMidiIn::alsaMidiHandler: error initializing buffer memory!\n\n";
    return 0;
  }
  snd_midi_event_init( apiData->coder );
  snd_midi_event_no_status( apiData->coder, 1 ); // suppress running status messages

  while ( data->doInput ) {

    if ( snd_seq_event_input_pending( apiData->seq, 1 ) == 0 ) {
      // No data pending ... sleep a bit.
      usleep( 1000 );
      continue;
    }

    // If here, there should be data.
    result = snd_seq_event_input( apiData->seq, &ev );
    if ( result == -ENOSPC ) {
      std::cerr << "\nRtMidiIn::alsaMidiHandler: MIDI input buffer overrun!\n\n";
      continue;
    }
    else if ( result <= 0 ) {
      std::cerr << "RtMidiIn::alsaMidiHandler: unknown MIDI input error!\n";
      continue;
    }

    // This is a bit weird, but we now have to decode an ALSA MIDI
    // event (back) into MIDI bytes.  We'll ignore non-MIDI types.
    message.bytes.clear();
    switch ( ev->type ) {

        case SND_SEQ_EVENT_PORT_SUBSCRIBED:
#if defined(__RTMIDI_DEBUG__)
      std::cout << "RtMidiIn::alsaMidiHandler: port connection made!\n";
#endif
      break;

        case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
      std::cerr << "RtMidiIn::alsaMidiHandler: port connection has closed!\n";
      data->doInput = false;
      break;

    case SND_SEQ_EVENT_QFRAME: // MIDI time code
      if ( data->ignoreFlags & 0x02 ) break;

    case SND_SEQ_EVENT_TICK: // MIDI timing tick
      if ( data->ignoreFlags & 0x02 ) break;

    case SND_SEQ_EVENT_SENSING: // Active sensing
      if ( data->ignoreFlags & 0x04 ) break;

        case SND_SEQ_EVENT_SYSEX:
      if ( (data->ignoreFlags & 0x01) ) break;
      if ( ev->data.ext.len > apiData->bufferSize ) {
        apiData->bufferSize = ev->data.ext.len;
        free( buffer );
        buffer = (unsigned char *) malloc( apiData->bufferSize );
        if ( buffer == NULL ) {
          data->doInput = false;
          std::cerr << "\nRtMidiIn::alsaMidiHandler: error resizing buffer memory!\n\n";
          break;
        }
      }

    default:
      nBytes = snd_midi_event_decode( apiData->coder, buffer, apiData->bufferSize, ev );
      if ( nBytes <= 0 ) {
#if defined(__RTMIDI_DEBUG__)
        std::cerr << "\nRtMidiIn::alsaMidiHandler: event parsing error or not a MIDI event!\n\n";
#endif
        break;
      }

      // The ALSA sequencer has a maximum buffer size for MIDI sysex
      // events of 256 bytes.  If a device sends sysex messages larger
      // than this, they are segmented into 256 byte chunks.  So,
      // we'll watch for this and concatenate sysex chunks into a
      // single sysex message if necessary.
      if ( !continueSysex )
        message.bytes.assign( buffer, &buffer[nBytes] );
      else
        message.bytes.insert( message.bytes.end(), buffer, &buffer[nBytes] );

      if ( ev->type == SND_SEQ_EVENT_SYSEX && message.bytes.back() == 0xF7 )
        continueSysex = false;
      else {
        continueSysex = true;
        break;
      }

      // Calculate the time stamp:
      message.timeStamp = 0.0;

      // Method 1: Use the system time.
      //(void)gettimeofday(&tv, (struct timezone *)NULL);
      //time = (tv.tv_sec * 1000000) + tv.tv_usec;

      // Method 2: Use the ALSA sequencer event time data.
      // (thanks to Pedro Lopez-Cabanillas!).
      time = ( ev->time.time.tv_sec * 1000000 ) + ( ev->time.time.tv_nsec/1000 );
      lastTime = time;
      time -= apiData->lastTime;
      apiData->lastTime = lastTime;
      if ( data->firstMessage == true )
        data->firstMessage = false;
      else
        message.timeStamp = time * 0.000001;
    }

    snd_seq_free_event(ev);
    if ( message.bytes.size() == 0 ) continue;

    if ( data->usingCallback ) {
      RtMidiIn::RtMidiCallback callback = (RtMidiIn::RtMidiCallback) data->userCallback;
      callback( message.timeStamp, &message.bytes, data->userData );
    }
    else {
      // As long as we haven't reached our queue size limit, push the message.
      if ( data->queueLimit > data->queue.size() )
        data->queue.push( message );
      else
        std::cerr << "\nRtMidiIn: message queue limit reached!!\n\n";
    }
  }

  if ( buffer ) free( buffer );
  snd_midi_event_free( apiData->coder );
  apiData->coder = 0;
  return 0;
}

void RtMidiIn :: initialize( void )
{
  // Set up the ALSA sequencer client.
    snd_seq_t *seq;
  int result = snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
  if ( result < 0 ) {
    errorString_ = "RtMidiIn::initialize: error creating ALSA sequencer input client object.";
    error( RtMidiError::DRIVER_ERROR );
    }

  // Set client name.
  snd_seq_set_client_name(seq, "RtMidi Input Client");

  // Save our api-specific connection information.
  AlsaMidiData *data = (AlsaMidiData *) new AlsaMidiData;
  data->seq = seq;
  data->vport = -1;
  apiData_ = (void *) data;
  inputData_.apiData = (void *) data;

  // Create the input queue
  data->queue_id = snd_seq_alloc_named_queue(seq, "RtMidi Queue");
  // Set arbitrary tempo (mm=100) and resolution (240)
  snd_seq_queue_tempo_t *qtempo;
  snd_seq_queue_tempo_alloca(&qtempo);
  snd_seq_queue_tempo_set_tempo(qtempo, 600000);
  snd_seq_queue_tempo_set_ppq(qtempo, 240);
  snd_seq_set_queue_tempo(data->seq, data->queue_id, qtempo);
  snd_seq_drain_output(data->seq);
}

// This function is used to count or get the pinfo structure for a given port number.
unsigned int portInfo( snd_seq_t *seq, snd_seq_port_info_t *pinfo, unsigned int type, int portNumber )
{
    snd_seq_client_info_t *cinfo;
  int client;
  int count = 0;
    snd_seq_client_info_alloca( &cinfo );

    snd_seq_client_info_set_client( cinfo, -1 );
    while ( snd_seq_query_next_client( seq, cinfo ) >= 0 ) {
    client = snd_seq_client_info_get_client( cinfo );
    if ( client == 0 ) continue;
        // Reset query info
        snd_seq_port_info_set_client( pinfo, client );
        snd_seq_port_info_set_port( pinfo, -1 );
        while ( snd_seq_query_next_port( seq, pinfo ) >= 0 ) {
      if ( !PORT_TYPE( pinfo, type ) )  continue;
      if ( count == portNumber ) return 1;
      count++;
        }
    }

  // If a negative portNumber was used, return the port count.
  if ( portNumber < 0 ) return count;
  return 0;
}

void RtMidiIn :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiIn::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  unsigned int nSrc = this->getPortCount();
  if (nSrc < 1) {
    errorString_ = "RtMidiIn::openPort: no MIDI input sources found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );
  std::ostringstream ost;
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, (int) portNumber ) == 0 ) {
    ost << "RtMidiIn::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }


  snd_seq_addr_t sender, receiver;
  sender.client = snd_seq_port_info_get_client( pinfo );
  sender.port = snd_seq_port_info_get_port( pinfo );
  receiver.client = snd_seq_client_id( data->seq );
  if ( data->vport < 0 ) {
    snd_seq_port_info_set_client( pinfo, 0 );
    snd_seq_port_info_set_port( pinfo, 0 );
    snd_seq_port_info_set_capability( pinfo,
                                      SND_SEQ_PORT_CAP_WRITE |
                                      SND_SEQ_PORT_CAP_SUBS_WRITE );
    snd_seq_port_info_set_type( pinfo,
                                SND_SEQ_PORT_TYPE_MIDI_GENERIC |
                                SND_SEQ_PORT_TYPE_APPLICATION );
    snd_seq_port_info_set_midi_channels(pinfo, 16);
    snd_seq_port_info_set_timestamping(pinfo, 1);
    snd_seq_port_info_set_timestamp_real(pinfo, 1);    
    snd_seq_port_info_set_timestamp_queue(pinfo, data->queue_id);
    snd_seq_port_info_set_name(pinfo, "RtMidi Input");
    data->vport = snd_seq_create_port(data->seq, pinfo);
  
    if ( data->vport < 0 ) {
      errorString_ = "RtMidiIn::openPort: ALSA error creating input port.";
      error( RtMidiError::DRIVER_ERROR );
    }
  }

  receiver.port = data->vport;

  // Make subscription
  snd_seq_port_subscribe_malloc( &data->subscription );
  snd_seq_port_subscribe_set_sender(data->subscription, &sender);
  snd_seq_port_subscribe_set_dest(data->subscription, &receiver);
  if ( snd_seq_subscribe_port(data->seq, data->subscription) ) {
    errorString_ = "RtMidiIn::openPort: ALSA error making port connection.";
    error( RtMidiError::DRIVER_ERROR );
  }

  if ( inputData_.doInput == false ) {
    // Start the input queue
    snd_seq_start_queue( data->seq, data->queue_id, NULL );
    snd_seq_drain_output( data->seq );
    // Start our MIDI input thread.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // chuck
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    //pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

    inputData_.doInput = true;
    int err = pthread_create(&data->thread, &attr, alsaMidiHandler, &inputData_);
    pthread_attr_destroy(&attr);
    if (err) {
      snd_seq_unsubscribe_port( data->seq, data->subscription );
      snd_seq_port_subscribe_free( data->subscription );
      inputData_.doInput = false;
      errorString_ = "RtMidiIn::openPort: error starting MIDI input thread!";
      error( RtMidiError::THREAD_ERROR );
    }
  }

  connected_ = true;
}

void RtMidiIn :: openVirtualPort()
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( data->vport < 0 ) {
    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );
    snd_seq_port_info_set_capability( pinfo,
                      SND_SEQ_PORT_CAP_WRITE |
                      SND_SEQ_PORT_CAP_SUBS_WRITE );
    snd_seq_port_info_set_type( pinfo,
                SND_SEQ_PORT_TYPE_MIDI_GENERIC |
                SND_SEQ_PORT_TYPE_APPLICATION );
    snd_seq_port_info_set_midi_channels(pinfo, 16);
    snd_seq_port_info_set_timestamping(pinfo, 1);
    snd_seq_port_info_set_timestamp_real(pinfo, 1);    
    snd_seq_port_info_set_timestamp_queue(pinfo, data->queue_id);
    snd_seq_port_info_set_name(pinfo, "RtMidi Input");
    data->vport = snd_seq_create_port(data->seq, pinfo);

    if ( data->vport < 0 ) {
      errorString_ = "RtMidiIn::openVirtualPort: ALSA error creating virtual port.";
      error( RtMidiError::DRIVER_ERROR );
    }
  }

  if ( inputData_.doInput == false ) {
    // Start the input queue
    snd_seq_start_queue( data->seq, data->queue_id, NULL );
    snd_seq_drain_output( data->seq );
    // Start our MIDI input thread.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // chuck
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    //pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

    inputData_.doInput = true;
    int err = pthread_create(&data->thread, &attr, alsaMidiHandler, &inputData_);
    pthread_attr_destroy(&attr);
    if (err) {
      snd_seq_unsubscribe_port( data->seq, data->subscription );
      snd_seq_port_subscribe_free( data->subscription );
      inputData_.doInput = false;
      errorString_ = "RtMidiIn::openPort: error starting MIDI input thread!";
      error( RtMidiError::THREAD_ERROR );
    }
  }
}

void RtMidiIn :: closePort( void )
{
  if ( connected_ ) {
    AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
    snd_seq_unsubscribe_port( data->seq, data->subscription );
    snd_seq_port_subscribe_free( data->subscription );
    // Stop the input queue
    snd_seq_stop_queue( data->seq, data->queue_id, NULL );
    snd_seq_drain_output( data->seq );
    connected_ = false;
  }
}

RtMidiIn :: ~RtMidiIn()
{
  // Close a connection if it exists.
  closePort();

  // Shutdown the input thread.
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( inputData_.doInput ) {
    inputData_.doInput = false;
    pthread_join( data->thread, NULL );
  }

  // Cleanup.
  if ( data->vport >= 0 ) snd_seq_delete_port( data->seq, data->vport );
  snd_seq_free_queue( data->seq, data->queue_id );
  snd_seq_close( data->seq );
  delete data;
}

unsigned int RtMidiIn :: getPortCount()
{
    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );

  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  return portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, -1 );
}

std::string RtMidiIn :: getPortName( unsigned int portNumber )
{
    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );

  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, (int) portNumber ) ) {
    std::string stringName = std::string( snd_seq_port_info_get_name( pinfo ) );
    return stringName;
  }

  // If we get here, we didn't find a match.
  errorString_ = "RtMidiIn::getPortName: error looking for port name!";
  error( RtMidiError::INVALID_PARAMETER );
  return 0;
}

//*********************************************************************//
//  API: LINUX ALSA
//  Class Definitions: RtMidiOut
//*********************************************************************//

unsigned int RtMidiOut :: getPortCount()
{
    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );

  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  return portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, -1 );
}

std::string RtMidiOut :: getPortName( unsigned int portNumber )
{
    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );

  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, (int) portNumber ) ) {
    std::string stringName = std::string( snd_seq_port_info_get_name( pinfo ) );
    return stringName;
  }

  // If we get here, we didn't find a match.
  errorString_ = "RtMidiOut::getPortName: error looking for port name!";
  error( RtMidiError::INVALID_PARAMETER );
  return 0;
}

void RtMidiOut :: initialize( void )
{
  // Set up the ALSA sequencer client.
    snd_seq_t *seq;
  int result = snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
  if ( result < 0 ) {
    errorString_ = "RtMidiOut::initialize: error creating ALSA sequencer client object.";
    error( RtMidiError::DRIVER_ERROR );
    }

  // Set client name.
  snd_seq_set_client_name(seq, "RtMidi Output Client");

  // Save our api-specific connection information.
  AlsaMidiData *data = (AlsaMidiData *) new AlsaMidiData;
  data->seq = seq;
  data->vport = -1;
  data->bufferSize = 32;
  data->coder = 0;
  data->buffer = 0;
  result = snd_midi_event_new( data->bufferSize, &data->coder );
  if ( result < 0 ) {
    delete data;
    errorString_ = "RtMidiOut::initialize: error initializing MIDI event parser!\n\n";
    error( RtMidiError::DRIVER_ERROR );
  }
  data->buffer = (unsigned char *) malloc( data->bufferSize );
  if ( data->buffer == NULL ) {
    delete data;
    errorString_ = "RtMidiOut::initialize: error allocating buffer memory!\n\n";
    error( RtMidiError::MEMORY_ERROR );
  }
  snd_midi_event_init( data->coder );
  apiData_ = (void *) data;
}

void RtMidiOut :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiOut::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  unsigned int nSrc = this->getPortCount();
  if (nSrc < 1) {
    errorString_ = "RtMidiOut::openPort: no MIDI output sources found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );
  std::ostringstream ost;
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, (int) portNumber ) == 0 ) {
    ost << "RtMidiOut::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  snd_seq_addr_t sender, receiver;
  receiver.client = snd_seq_port_info_get_client( pinfo );
  receiver.port = snd_seq_port_info_get_port( pinfo );
  sender.client = snd_seq_client_id( data->seq );

  if ( data->vport < 0 ) {
    data->vport = snd_seq_create_simple_port( data->seq, "RtMidi Output",
                                              SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
                                              SND_SEQ_PORT_TYPE_MIDI_GENERIC );
    if ( data->vport < 0 ) {
      errorString_ = "RtMidiOut::openPort: ALSA error creating output port.";
      error( RtMidiError::DRIVER_ERROR );
    }
  }

  sender.port = data->vport;

  // Make subscription
  snd_seq_port_subscribe_malloc( &data->subscription );
  snd_seq_port_subscribe_set_sender(data->subscription, &sender);
  snd_seq_port_subscribe_set_dest(data->subscription, &receiver);
  snd_seq_port_subscribe_set_time_update(data->subscription, 1);
  snd_seq_port_subscribe_set_time_real(data->subscription, 1);
  if ( snd_seq_subscribe_port(data->seq, data->subscription) ) {
    errorString_ = "RtMidiOut::openPort: ALSA error making port connection.";
    error( RtMidiError::DRIVER_ERROR );
  }

  connected_ = true;
}

void RtMidiOut :: closePort( void )
{
  if ( connected_ ) {
    AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
    snd_seq_unsubscribe_port( data->seq, data->subscription );
    snd_seq_port_subscribe_free( data->subscription );
    connected_ = false;
  }
}

void RtMidiOut :: openVirtualPort()
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( data->vport < 0 ) {
    data->vport = snd_seq_create_simple_port( data->seq, "RtMidi Output",
                                              SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
                                              SND_SEQ_PORT_TYPE_MIDI_GENERIC );

    if ( data->vport < 0 ) {
      errorString_ = "RtMidiOut::openVirtualPort: ALSA error creating virtual port.";
      error( RtMidiError::DRIVER_ERROR );
    }
  }
}

RtMidiOut :: ~RtMidiOut()
{
  // Close a connection if it exists.
  closePort();

  // Cleanup.
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( data->vport >= 0 ) snd_seq_delete_port( data->seq, data->vport );
  if ( data->coder ) snd_midi_event_free( data->coder );
  if ( data->buffer ) free( data->buffer );
  snd_seq_close( data->seq );
  delete data;
}

void RtMidiOut :: sendMessage( std::vector<unsigned char> *message )
{
  int result;
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  unsigned int nBytes = message->size();
  if ( nBytes > data->bufferSize ) {
    data->bufferSize = nBytes;
    result = snd_midi_event_resize_buffer ( data->coder, nBytes);
    if ( result != 0 ) {
      errorString_ = "RtMidiOut::sendMessage: ALSA error resizing MIDI event buffer.";
      error( RtMidiError::DRIVER_ERROR );
    }
    free (data->buffer);
    data->buffer = (unsigned char *) malloc( data->bufferSize );
    if ( data->buffer == NULL ) {
    errorString_ = "RtMidiOut::initialize: error allocating buffer memory!\n\n";
    error( RtMidiError::MEMORY_ERROR );
    }
  }

  snd_seq_event_t ev;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, data->vport);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_direct(&ev);
  for ( unsigned int i=0; i<nBytes; i++ ) data->buffer[i] = message->at(i);
  result = snd_midi_event_encode( data->coder, data->buffer, (long)nBytes, &ev );
  if ( result < (int)nBytes ) {
    errorString_ = "RtMidiOut::sendMessage: event parsing error!";
    error( RtMidiError::WARNING );
    return;
  }

  // Send the event.
  result = snd_seq_event_output(data->seq, &ev);
  if ( result < 0 ) {
    errorString_ = "RtMidiOut::sendMessage: error sending MIDI message to port.";
    error( RtMidiError::WARNING );
  }
  snd_seq_drain_output(data->seq);
}

#endif // __LINUX_ALSA__


//*********************************************************************//
//  API: IRIX MD
//*********************************************************************//

// API information gleamed from:
//   http://techpubs.sgi.com/library/tpl/cgi-bin/getdoc.cgi?cmd=getdoc&coll=0650&db=man&fname=3%20mdIntro

// If the Makefile doesn't work, try the following:
// CC -o midiinfo -LANG:std -D__IRIX_MD__ -I../ ../RtMidi.cpp midiinfo.cpp -lpthread -lmd
// CC -o midiout -LANG:std -D__IRIX_MD__ -I../ ../RtMidi.cpp midiout.cpp -lpthread -lmd
// CC -o qmidiin -LANG:std -D__IRIX_MD__ -I../ ../RtMidi.cpp qmidiin.cpp -lpthread -lmd
// CC -o cmidiin -LANG:std -D__IRIX_MD__ -I../ ../RtMidi.cpp cmidiin.cpp -lpthread -lmd

#if defined(__IRIX_MD__)

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

// Irix MIDI header file.
#include <dmedia/midi.h>

// A structure to hold variables related to the IRIX API
// implementation.
struct IrixMidiData {
  MDport port;
  pthread_t thread;
};

//*********************************************************************//
//  API: IRIX
//  Class Definitions: RtMidiIn
//*********************************************************************//

extern "C" void *irixMidiHandler( void *ptr )
{
  RtMidiIn::RtMidiInData *data = static_cast<RtMidiIn::RtMidiInData *> (ptr);
  IrixMidiData *apiData = static_cast<IrixMidiData *> (data->apiData);

  bool continueSysex = false;
  unsigned char status;
  unsigned short size;
  MDevent event;
  int fd = mdGetFd( apiData->port );
  if ( fd < 0 ) {
    data->doInput = false;
    std::cerr << "\nRtMidiIn::irixMidiHandler: error getting port descriptor!\n\n";
    return 0;
  }

  fd_set mask, rmask;
  FD_ZERO( &mask );
  FD_SET( fd, &mask );
  struct timeval timeout = {0, 0};
  RtMidiIn::MidiMessage message;
  int result;

  while ( data->doInput ) {

    rmask = mask;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    if ( select( fd+1, &rmask, NULL, NULL, &timeout ) <= 0 ) {
      // No data pending ... sleep a bit.
      usleep( 1000 );
      continue;
    }

    // If here, there should be data.
    result = mdReceive( apiData->port, &event, 1);
    if ( result <= 0 ) {
      std::cerr << "\nRtMidiIn::irixMidiHandler: MIDI input read error!\n\n";
      continue;
    }

    message.timeStamp = event.stamp * 0.000000001;

    size = 0;
    status = event.msg[0];
    if ( !(status & 0x80) ) continue;
    if ( status == 0xF0 ) {
      // Sysex message ... can be segmented across multiple messages.
      if ( !(data->ignoreFlags & 0x01) ) {
        if ( continueSysex ) {
          // We have a continuing, segmented sysex message.  Append
          // the new bytes to our existing message.
          for ( int i=0; i<event.msglen; i++ )
            message.bytes.push_back( event.sysexmsg[i] );
          if ( event.sysexmsg[event.msglen-1] == 0xF7 ) continueSysex = false;
          if ( !continueSysex ) {
            // If not a continuing sysex message, invoke the user callback function or queue the message.
            if ( data->usingCallback && message.bytes.size() > 0 ) {
              RtMidiIn::RtMidiCallback callback = (RtMidiIn::RtMidiCallback) data->userCallback;
              callback( message.timeStamp, &message.bytes, data->userData );
            }
            else {
              // As long as we haven't reached our queue size limit, push the message.
              if ( data->queueLimit > data->queue.size() )
                data->queue.push( message );
              else
                std::cerr << "\nRtMidiIn: message queue limit reached!!\n\n";
            }
            message.bytes.clear();
          }
        }
      }
      mdFree( NULL );
      continue;
    }
    else if ( status < 0xC0 ) size = 3;
    else if ( status < 0xE0 ) size = 2;
    else if ( status < 0xF0 ) size = 3;
    else if ( status < 0xF3 ) {
      if ( status == 0xF1 && !(data->ignoreFlags & 0x02) ) {
        // A MIDI time code message and we're not ignoring it.
        size = 3;
      }
    }
    else if ( status == 0xF3 ) size = 2;
    else if ( status == 0xF8 ) {
      if ( !(data->ignoreFlags & 0x02) ) {
        // A MIDI timing tick message and we're not ignoring it.
        size = 1;
      }
    }
    else if ( status == 0xFE ) { // MIDI active sensing
      if ( !(data->ignoreFlags & 0x04) )
        size = 1;
    }
    else size = 1;

    // Copy the MIDI data to our vector.
    if ( size ) {
      message.bytes.assign( &event.msg[0], &event.msg[size] );
      // Invoke the user callback function or queue the message.
      if ( data->usingCallback ) {
        RtMidiIn::RtMidiCallback callback = (RtMidiIn::RtMidiCallback) data->userCallback;
        callback( message.timeStamp, &message.bytes, data->userData );
      }
      else {
        // As long as we haven't reached our queue size limit, push the message.
        if ( data->queueLimit > data->queue.size() )
          data->queue.push( message );
        else
          std::cerr << "\nRtMidiIn: message queue limit reached!!\n\n";
      }
      message.bytes.clear();
    }
  }

  return 0;
}

void RtMidiIn :: initialize( void )
{
  // Initialize the Irix MIDI system.  At the moment, we will not
  // worry about a return value of zero (ports) because there is a
  // chance the user could plug something in after instantiation.
  int nPorts = mdInit();

  // Create our api-specific connection information.
  IrixMidiData *data = (IrixMidiData *) new IrixMidiData;
  apiData_ = (void *) data;
  inputData_.apiData = (void *) data;
}

void RtMidiIn :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiIn::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  int nPorts = mdInit();
  if (nPorts < 1) {
    errorString_ = "RtMidiIn::openPort: no Irix MIDI input sources found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

  std::ostringstream ost;
  if ( portNumber >= nPorts ) {
    ost << "RtMidiIn::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  IrixMidiData *data = static_cast<IrixMidiData *> (apiData_);
  data->port = mdOpenInPort( mdGetName(portNumber) );
  if ( data->port == NULL ) {
    ost << "RtMidiIn::openPort: Irix error opening the port (" << portNumber << ").";
    errorString_ = ost.str();
    error( RtMidiError::DRIVER_ERROR );
  }
  mdSetStampMode(data->port, MD_DELTASTAMP);

  // Start our MIDI input thread.
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_attr_setschedpolicy(&attr, SCHED_RR);

  inputData_.doInput = true;
  int err = pthread_create(&data->thread, &attr, irixMidiHandler, &inputData_);
  pthread_attr_destroy(&attr);
  if (err) {
    mdClosePort( data->port );
    inputData_.doInput = false;
    errorString_ = "RtMidiIn::openPort: error starting MIDI input thread!";
    error( RtMidiError::THREAD_ERROR );
  }

  connected_ = true;
}

void RtMidiIn :: openVirtualPort()
{
  // This function cannot be implemented for the Irix MIDI API.
  errorString_ = "RtMidiIn::openVirtualPort: cannot be implemented in Irix MIDI API!";
  error( RtMidiError::WARNING );
}

void RtMidiIn :: closePort( void )
{
  if ( connected_ ) {
    IrixMidiData *data = static_cast<IrixMidiData *> (apiData_);
    mdClosePort( data->port );
    connected_ = false;

    // Shutdown the input thread.
    inputData_.doInput = false;
    pthread_join( data->thread, NULL );
  }
}

RtMidiIn :: ~RtMidiIn()
{
  // Close a connection if it exists.
  closePort();

  // Cleanup.
  IrixMidiData *data = static_cast<IrixMidiData *> (apiData_);
  delete data;
}

unsigned int RtMidiIn :: getPortCount()
{
  int nPorts = mdInit();
  if ( nPorts >= 0 ) return nPorts;
  else return 0;
}

std::string RtMidiIn :: getPortName( unsigned int portNumber )
{
  int nPorts = mdInit();

  std::ostringstream ost;
  if ( portNumber >= nPorts ) {
    ost << "RtMidiIn::getPortName: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  std::string stringName = std::string( mdGetName( portNumber ) );
  return stringName;
}

//*********************************************************************//
//  API: IRIX MD
//  Class Definitions: RtMidiOut
//*********************************************************************//

unsigned int RtMidiOut :: getPortCount()
{
  int nPorts = mdInit();
  if ( nPorts >= 0 ) return nPorts;
  else return 0;
}

std::string RtMidiOut :: getPortName( unsigned int portNumber )
{
  int nPorts = mdInit();

  std::ostringstream ost;
  if ( portNumber >= nPorts ) {
    ost << "RtMidiIn::getPortName: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  std::string stringName = std::string( mdGetName( portNumber ) );
  return stringName;
}

void RtMidiOut :: initialize( void )
{
  // Initialize the Irix MIDI system.  At the moment, we will not
  // worry about a return value of zero (ports) because there is a
  // chance the user could plug something in after instantiation.
  int nPorts = mdInit();

  // Create our api-specific connection information.
  IrixMidiData *data = (IrixMidiData *) new IrixMidiData;
  apiData_ = (void *) data;
}

void RtMidiOut :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiOut::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  int nPorts = mdInit();
  if (nPorts < 1) {
    errorString_ = "RtMidiOut::openPort: no Irix MIDI output sources found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

  std::ostringstream ost;
  if ( portNumber >= nPorts ) {
    ost << "RtMidiOut::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  IrixMidiData *data = static_cast<IrixMidiData *> (apiData_);
  data->port = mdOpenOutPort( mdGetName(portNumber) );
  if ( data->port == NULL ) {
    ost << "RtMidiOut::openPort: Irix error opening the port (" << portNumber << ").";
    errorString_ = ost.str();
    error( RtMidiError::DRIVER_ERROR );
  }
  mdSetStampMode(data->port, MD_NOSTAMP);

  connected_ = true;
}

void RtMidiOut :: closePort( void )
{
  if ( connected_ ) {
    IrixMidiData *data = static_cast<IrixMidiData *> (apiData_);
    mdClosePort( data->port );
    connected_ = false;
  }
}

void RtMidiOut :: openVirtualPort()
{
  // This function cannot be implemented for the Irix MIDI API.
  errorString_ = "RtMidiOut::openVirtualPort: cannot be implemented in Irix MIDI API!";
  error( RtMidiError::WARNING );
}

RtMidiOut :: ~RtMidiOut()
{
  // Close a connection if it exists.
  closePort();

  // Cleanup.
  IrixMidiData *data = static_cast<IrixMidiData *> (apiData_);
  delete data;
}

void RtMidiOut :: sendMessage( std::vector<unsigned char> *message )
{
  int result;
  MDevent event;
  IrixMidiData *data = static_cast<IrixMidiData *> (apiData_);
  char *buffer = 0;

  unsigned int nBytes = message->size();
  if ( nBytes == 0 ) return;
  event.stamp = 0;
  if ( message->at(0) == 0xF0 ) {
    if ( nBytes < 3 ) return; // check for bogus sysex
    event.msg[0] = 0xF0;
    event.msglen = nBytes;
    buffer = (char *) malloc( nBytes );
    for ( int i=0; i<nBytes; i++ ) buffer[i] = message->at(i);
    event.sysexmsg = buffer;
  }
  else {
    for ( int i=0; i<nBytes; i++ )
      event.msg[i] = message->at(i);
  }

  // Send the event.
  result = mdSend( data->port, &event, 1 );
  if ( buffer ) free( buffer );
  if ( result < 1 ) {
    errorString_ = "RtMidiOut::sendMessage: IRIX error sending MIDI message!";
    error( RtMidiError::WARNING );
    return;
  }
}

#endif // __IRIX_MD__

//*********************************************************************//
//  API: Windows Multimedia Library (MM)
//*********************************************************************//

// API information deciphered from:
//  - http://msdn.microsoft.com/library/default.asp?url=/library/en-us/multimed/htm/_win32_midi_reference.asp

// chuck
#if defined(__WINDOWS_DS__)
//#if defined(__WINDOWS_MM__)

// The Windows MM API is based on the use of a callback function for
// MIDI input.  We convert the system specific time stamps to delta
// time values.

// Windows MM MIDI header files.
#include <windows.h>
#include <mmsystem.h>

// A structure to hold variables related to the CoreMIDI API
// implementation.
struct WinMidiData {
  HMIDIIN inHandle;    // Handle to Midi Input Device
  HMIDIOUT outHandle;  // Handle to Midi Output Device
  DWORD lastTime;
  RtMidiIn::MidiMessage message;
};

//*********************************************************************//
//  API: Windows MM
//  Class Definitions: RtMidiIn
//*********************************************************************//

static void CALLBACK midiInputCallback( HMIDIOUT hmin,
                                        UINT inputStatus, 
                                        DWORD instancePtr,
                                        DWORD midiMessage,
                                        DWORD timestamp )
{
  if ( inputStatus != MIM_DATA && inputStatus != MIM_LONGDATA ) return;

  //RtMidiIn::RtMidiInData *data = static_cast<RtMidiIn::RtMidiInData *> (instancePtr);
  RtMidiIn::RtMidiInData *data = (RtMidiIn::RtMidiInData *)instancePtr;
  WinMidiData *apiData = static_cast<WinMidiData *> (data->apiData);

  // Calculate time stamp.
  apiData->message.timeStamp = 0.0;
  if ( data->firstMessage == true ) data->firstMessage = false;
  else apiData->message.timeStamp = (double) ( timestamp - apiData->lastTime ) * 0.001;
  apiData->lastTime = timestamp;

  if ( inputStatus == MIM_DATA ) { // Channel or system message

    // Make sure the first byte is a status byte.
    unsigned char status = (unsigned char) (midiMessage & 0x000000FF);
    if ( !(status & 0x80) ) return;

    // Determine the number of bytes in the MIDI message.
    unsigned short nBytes = 1;
    if ( status < 0xC0 ) nBytes = 3;
    else if ( status < 0xE0 ) nBytes = 2;
    else if ( status < 0xF0 ) nBytes = 3;
    else if ( status < 0xF3 ) {
      // A MIDI time code message and we're ignoring it.
      if ( status == 0xF1 && (data->ignoreFlags & 0x02) ) return;
      nBytes = 3;
    }
    else if ( status == 0xF3 ) nBytes = 2;
    else if ( status == 0xF8 && (data->ignoreFlags & 0x02) ) {
      // A MIDI timing tick message and we're ignoring it.
      return;
    }
    else if ( status == 0xFE && (data->ignoreFlags & 0x04) ) {
      // A MIDI active sensing message and we're ignoring it.
      return;
    }

    // Copy bytes to our MIDI message.
    unsigned char *ptr = (unsigned char *) &midiMessage;
    for ( int i=0; i<nBytes; i++ ) apiData->message.bytes.push_back( *ptr++ );
  }
  else { // Sysex message
    MIDIHDR *sysex = ( MIDIHDR *) midiMessage;
    for ( int i=0; i<(int)sysex->dwBytesRecorded; i++ )
      apiData->message.bytes.push_back( sysex->lpData[i] );
    if ( apiData->message.bytes.back() != 0xF7 ) return;
  }

  if ( data->usingCallback ) {
    RtMidiIn::RtMidiCallback callback = (RtMidiIn::RtMidiCallback) data->userCallback;
    callback( apiData->message.timeStamp, &apiData->message.bytes, data->userData );
  }
  else {
    // As long as we haven't reached our queue size limit, push the message.
    if ( data->queueLimit > data->queue.size() )
      data->queue.push( apiData->message );
    else
      std::cerr << "\nRtMidiIn: message queue limit reached!!\n\n";
  }

  // Clear the vector for the next input message.  Note that doing
  // this here allows our code to work for sysex messages which are
  // segmented across multiple buffers.
  apiData->message.bytes.clear();
}

void RtMidiIn :: initialize( void )
{
  // We'll issue a warning here if no devices are available but not
  // throw an error since the user can plugin something later.
  unsigned int nDevices = midiInGetNumDevs();
  if ( nDevices == 0 ) {
    errorString_ = "RtMidiIn::initialize: no MIDI input devices currently available.";
    error( RtMidiError::WARNING );
  }

  // Save our api-specific connection information.
  WinMidiData *data = (WinMidiData *) new WinMidiData;
  apiData_ = (void *) data;
  inputData_.apiData = (void *) data;
  data->message.bytes.clear();  // needs to be empty for first input message
}

void RtMidiIn :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiIn::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  unsigned int nDevices = midiInGetNumDevs();
  if (nDevices == 0) {
    errorString_ = "RtMidiIn::openPort: no MIDI input sources found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

  std::ostringstream ost;
  if ( portNumber >= nDevices ) {
    ost << "RtMidiIn::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  MMRESULT result = midiInOpen( &data->inHandle,
                                portNumber,
                                (DWORD)&midiInputCallback,
                                (DWORD)&inputData_,
                                CALLBACK_FUNCTION );
  if ( result != MMSYSERR_NOERROR ) {
    errorString_ = "RtMidiIn::openPort: error creating Windows MM MIDI input port.";
    error( RtMidiError::DRIVER_ERROR );
  }

  result = midiInStart( data->inHandle );
  if ( result != MMSYSERR_NOERROR ) {
    midiInClose( data->inHandle );
    errorString_ = "RtMidiIn::openPort: error starting Windows MM MIDI input port.";
    error( RtMidiError::DRIVER_ERROR );
  }

  connected_ = true;
}

void RtMidiIn :: openVirtualPort()
{
  // This function cannot be implemented for the Windows MM MIDI API.
  errorString_ = "RtMidiIn::openVirtualPort: cannot be implemented in Windows MM MIDI API!";
  error( RtMidiError::WARNING );
}

void RtMidiIn :: closePort( void )
{
  if ( connected_ ) {
    WinMidiData *data = static_cast<WinMidiData *> (apiData_);
    midiInReset( data->inHandle );
    midiInStop( data->inHandle );
    midiInClose( data->inHandle );
    connected_ = false;
  }
}

RtMidiIn :: ~RtMidiIn()
{
  // Close a connection if it exists.
  closePort();

  // Cleanup.
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  delete data;
}

unsigned int RtMidiIn :: getPortCount()
{
  return midiInGetNumDevs();
}

std::string RtMidiIn :: getPortName( unsigned int portNumber )
{
  unsigned int nDevices = midiInGetNumDevs();
  if ( portNumber >= nDevices ) {
    std::ostringstream ost;
    ost << "RtMidiIn::getPortName: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  MIDIINCAPS deviceCaps;
  MMRESULT result = midiInGetDevCaps( portNumber, &deviceCaps, sizeof(MIDIINCAPS));

  std::string stringName = std::string( deviceCaps.szPname );
  return stringName;
}

//*********************************************************************//
//  API: Windows MM
//  Class Definitions: RtMidiOut
//*********************************************************************//

unsigned int RtMidiOut :: getPortCount()
{
  return midiOutGetNumDevs();
}

std::string RtMidiOut :: getPortName( unsigned int portNumber )
{
  unsigned int nDevices = midiOutGetNumDevs();
  if ( portNumber >= nDevices ) {
    std::ostringstream ost;
    ost << "RtMidiOut::getPortName: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  MIDIOUTCAPS deviceCaps;
  MMRESULT result = midiOutGetDevCaps( portNumber, &deviceCaps, sizeof(MIDIOUTCAPS));

  std::string stringName = std::string( deviceCaps.szPname );
  return stringName;
}

void RtMidiOut :: initialize( void )
{
  // We'll issue a warning here if no devices are available but not
  // throw an error since the user can plug something in later.
  unsigned int nDevices = midiOutGetNumDevs();
  if ( nDevices == 0 ) {
    errorString_ = "RtMidiOut::initialize: no MIDI output devices currently available.";
    error( RtMidiError::WARNING );
  }

  // Save our api-specific connection information.
  WinMidiData *data = (WinMidiData *) new WinMidiData;
  apiData_ = (void *) data;
}

void RtMidiOut :: openPort( unsigned int portNumber )
{
  if ( connected_ ) {
    errorString_ = "RtMidiOut::openPort: a valid connection already exists!";
    error( RtMidiError::WARNING );
    return;
  }

  unsigned int nDevices = midiOutGetNumDevs();
  if (nDevices < 1) {
    errorString_ = "RtMidiOut::openPort: no MIDI output destinations found!";
    error( RtMidiError::NO_DEVICES_FOUND );
  }

  std::ostringstream ost;
  if ( portNumber >= nDevices ) {
    ost << "RtMidiOut::openPort: 'portNumber' argument (" << portNumber << ") is invalid.";
    errorString_ = ost.str();
    error( RtMidiError::INVALID_PARAMETER );
  }

  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  MMRESULT result = midiOutOpen( &data->outHandle,
                                 portNumber,
                                 (DWORD)NULL,
                                 (DWORD)NULL,
                                 CALLBACK_NULL );
  if ( result != MMSYSERR_NOERROR ) {
    errorString_ = "RtMidiOut::openPort: error creating Windows MM MIDI output port.";
    error( RtMidiError::DRIVER_ERROR );
  }

  connected_ = true;
}

void RtMidiOut :: closePort( void )
{
  if ( connected_ ) {
    WinMidiData *data = static_cast<WinMidiData *> (apiData_);
    midiOutReset( data->outHandle );
    midiOutClose( data->outHandle );
    connected_ = false;
  }
}

void RtMidiOut :: openVirtualPort()
{
  // This function cannot be implemented for the Windows MM MIDI API.
  errorString_ = "RtMidiOut::openVirtualPort: cannot be implemented in Windows MM MIDI API!";
  error( RtMidiError::WARNING );
}

RtMidiOut :: ~RtMidiOut()
{
  // Close a connection if it exists.
  closePort();

  // Cleanup.
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  delete data;
}

void RtMidiOut :: sendMessage( std::vector<unsigned char> *message )
{
  unsigned int nBytes = message->size();
  if ( nBytes == 0 ) {
    errorString_ = "RtMidiOut::sendMessage: message argument is empty!";
    error( RtMidiError::WARNING );
    return;
  }

  MMRESULT result;
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  if ( message->at(0) == 0xF0 ) { // Sysex message

    // Allocate buffer for sysex data.
    char *buffer = (char *) malloc( nBytes );
    if ( buffer == NULL ) {
      errorString_ = "RtMidiOut::sendMessage: error allocating sysex message memory!";
      error( RtMidiError::MEMORY_ERROR );
    }

    // Copy data to buffer.
    for ( unsigned int i=0; i<nBytes; i++ ) buffer[i] = message->at(i);

    // Create and prepare MIDIHDR structure.
    MIDIHDR sysex;
    sysex.lpData = (LPSTR) buffer;
    sysex.dwBufferLength = nBytes;
    sysex.dwFlags = 0;
    result = midiOutPrepareHeader( data->outHandle,  &sysex, sizeof(MIDIHDR) ); 
    if ( result != MMSYSERR_NOERROR ) {
      free( buffer );
      errorString_ = "RtMidiOut::sendMessage: error preparing sysex header.";
      error( RtMidiError::DRIVER_ERROR );
    }

    // Send the message.
    result = midiOutLongMsg( data->outHandle, &sysex, sizeof(MIDIHDR) );
    if ( result != MMSYSERR_NOERROR ) {
      free( buffer );
      errorString_ = "RtMidiOut::sendMessage: error sending sysex message.";
      error( RtMidiError::DRIVER_ERROR );
    }

    // Unprepare the buffer and MIDIHDR.
    while ( MIDIERR_STILLPLAYING == midiOutUnprepareHeader( data->outHandle, &sysex, sizeof (MIDIHDR) ) ) Sleep( 1 );
    free( buffer );

  }
  else { // Channel or system message.

    // Make sure the message size isn't too big.
    if ( nBytes > 3 ) {
      errorString_ = "RtMidiOut::sendMessage: message size is greater than 3 bytes (and not sysex)!";
      error( RtMidiError::WARNING );
      return;
    }

    // Pack MIDI bytes into double word.
    DWORD packet;
    unsigned char *ptr = (unsigned char *) &packet;
    for ( unsigned int i=0; i<nBytes; i++ ) {
      *ptr = message->at(i);
      ptr++;
    }

    // Send the message immediately.
    result = midiOutShortMsg( data->outHandle, packet );
    if ( result != MMSYSERR_NOERROR ) {
      errorString_ = "RtMidiOut::sendMessage: error sending MIDI message.";
      error( RtMidiError::DRIVER_ERROR );
    }
  }
}

#endif  // __WINDOWS_MM__


#if defined(__LINUX_OSS__)  // dummy

/*void midiInputCallback( const MIDIPacketList *list, void *procRef, void *srcRef )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}
*/
void RtMidiIn :: initialize( void )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

void RtMidiIn :: openPort( unsigned int portNumber )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

void RtMidiIn :: openVirtualPort()
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

void RtMidiIn :: closePort( void )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

RtMidiIn :: ~RtMidiIn()
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

unsigned int RtMidiIn :: getPortCount()
{
    return 0;
}

std::string RtMidiIn :: getPortName( unsigned int portNumber )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
    return "";
}

//*********************************************************************//
//  API: OS-X
//  Class Definitions: RtMidiOut
//*********************************************************************//

unsigned int RtMidiOut :: getPortCount()
{
    return 0;
}

std::string RtMidiOut :: getPortName( unsigned int portNumber )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
    return "";
}

void RtMidiOut :: initialize( void )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

void RtMidiOut :: openPort( unsigned int portNumber )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

void RtMidiOut :: closePort( void )
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

void RtMidiOut :: openVirtualPort()
{
    fprintf( stderr, "[chuck](via rtmidi): midi not supported for this platform\n" );
}

RtMidiOut :: ~RtMidiOut()
{
}

void RtMidiOut :: sendMessage( std::vector<unsigned char> *message )
{
}

#endif  // dummy
