//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

enum ControlType
{
    None = -1; // mode is undefined
    MaxAdvance = 0; 
    EndOfStep = 1; 
    Until = 2;
    Disconnect = 3;
    Reconnect = 4; 
    ContinueWaiting = 5;
    Traffic = 6;
}
