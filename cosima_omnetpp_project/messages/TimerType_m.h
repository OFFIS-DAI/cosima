//
// Generated file, do not edit! Created by nedtool 5.6 from messages/TimerType.msg.
//

#ifndef __TIMERTYPE_M_H
#define __TIMERTYPE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Enum generated from <tt>messages/TimerType.msg:17</tt> by nedtool.
 * <pre>
 * enum TimerType
 * {
 *     NoType = -1; // mode is undefined
 *     Connect = 0;
 *     Send = 1;
 *     Close = 2;
 *     Renew = 3;
 *     ConnectTimeout = 4;
 * }
 * </pre>
 */
enum TimerType {
    NoType = -1,
    Connect = 0,
    Send = 1,
    Close = 2,
    Renew = 3,
    ConnectTimeout = 4
};

#endif // ifndef __TIMERTYPE_M_H

