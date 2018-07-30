# ---------------------------------------------------------------------------
# @section LICENSE
#  
# Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
#               All Rights Reserved
#  
# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software.
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
# DEALINGS IN THE SOFTWARE.
# ---------------------------------------------------------------------------
# @file filename.ext
# @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
# @author Eric Squires <eric.squires@gtri.gatech.edu>
# @version 1.0
# ---------------------------------------------------------------------------
# @brief A brief description.
# 
# @section DESCRIPTION
# A long description.
# ---------------------------------------------------------------------------
from scrimmage.proto import Frame_pb2
import google.protobuf.internal.decoder

def read_frames(frames_file, to_dataframe=False):
    ''' Returns a list of frames from a protobuf binary file

    The protobuf frames file is a series of frames (see here:
    https://developers.google.com/protocol-buffers/docs/techniques#streaming)
    which exist because we need to save multiple frames to a single file.
    Unfortunately, python bindings are not documented/available so we
    need to use the google protobuf internals. The message format is
    "size of message, message".

    See here for implementation details:
    http://stackoverflow.com/a/21772949
    https://groups.google.com/forum/#!msg/protobuf/4RydUI1HkSM/oHdYdKQ1h5kJ

    The first link contains the code used below with the exception that decoder
    is _DecodeVarint32, found at the 2nd link
    '''

    with open(frames_file, 'rb') as f:
        data = f.read()

    frames = []
    next_pos, pos = 0, 0
    i = 0

    if to_dataframe:
        contact_data = []

    while True:

        try:
            next_pos, pos = \
                google.protobuf.internal.decoder._DecodeVarint32(data, pos)
        except IndexError:
            break

        frame = Frame_pb2.Frame()

        try:
            frame.ParseFromString(data[pos:pos + next_pos])
        except:
            print('Error reading frames! Aborting data read-in.')
            break

        frames.append(frame)

        pos += next_pos

    return frames
