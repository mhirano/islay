/**
 @file Messenger.hpp
 @brief A Messenger class of multi-threaded environments in conformity with C++11. No additional library required.
 @author mhirano<masahiro.dll@gmail.com> (Large part of this code is originally developed by Shingo W. Kagami)
 */

#ifndef ISLAY_INTERTHREADMESSENGER_H
#define ISLAY_INTERTHREADMESSENGER_H

#include <mutex>

/**
 * An interface class for the message data to be passed by InterThreadMessenger.
 * Data members are to be added in the subclass of this. 
 */
class MsgData {
    template<class T> friend class InterThreadMessenger;
public:

    /**
     * Constructor of the message data to be overridden if needed.
     */
    MsgData() : seqno(0) {};

    /**
     * Destructor of the message data to be overridden if needed.
     */
    virtual ~MsgData() {};

    /**
     * Method to be overridden when a part of the message data needs
     * actual copy (By default, the pointers to the message data are
     * exchanged instead of actual data copy.  If this pointer
     * exchange is the desired behavior, this method does not neeed to
     * be overridden).
     * 
     * @param dst Pointer to the destination message data
     */
    virtual void copyTo(MsgData *dst) {}

private:
    unsigned int seqno;
};

/**
 * Class template of the inter thread messenger
 *
 * @tparam CustomMsgData A subclass of MsgData passed by the messenger.
 */
template<class CustomMsgData>
class InterThreadMessenger {
public:
    InterThreadMessenger() : master_seqno(0), closed(false) {
        msg_sender = new CustomMsgData();
        msg_buffer = new CustomMsgData();
        msg_receiver = new CustomMsgData();
    }

    ~InterThreadMessenger() {
        delete msg_sender;
        delete msg_buffer;
        delete msg_receiver;
    }

    /**
     * Returns the pointer to the sender's CustomMsgData buffer so
     * that the message data to be sent can be put into it.
     */
    CustomMsgData *prepareMsg() const {
        return msg_sender;
    }

    /**
     * Send the message by exchanging the pointers to the sender's
     * buffer and the intermediate buffer.
     */
    void send() {
        master_seqno++;
        {
            std::lock_guard<std::mutex> lock(mtx);
            msg_sender->seqno = master_seqno;
            swapPtr(&msg_sender, &msg_buffer);
        }
    }

    /**
     * Returns true iff the message in the intermediate buffer is
     * newer than the one in the receiver's buffer.
     */
    bool isUpdated() {
        return msg_buffer->seqno > msg_receiver->seqno;
    }

    /**
     * Receive the message by exchanging the pointers to the
     * intermediate buffer and the receiver's buffer.  If not
     * isUpdated(), the pointers remain unchanged.
     * Returns the pointer to the receiver's buffer after exchange if
     * isUpdated(); nullptr otherwise.
     */
    CustomMsgData *receive() {
        if (!isUpdated()) {
            return nullptr;
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            swapPtr(&msg_buffer, &msg_receiver);
        }
        return msg_receiver;
    }

    /**
     * Returns true iff the messenger has been closed.
     */
    bool isClosed() const {
        return closed;
    }

    /**
     * Close the messenger.
     */
    void close() {
        closed = true;
    }
    
private:
    void swapPtr(CustomMsgData **p1, CustomMsgData **p2) {
        CustomMsgData *tmp;
        tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
    }
    
    CustomMsgData *msg_sender;
    CustomMsgData *msg_buffer;
    CustomMsgData *msg_receiver;
    std::mutex mtx;
    unsigned int master_seqno;
    bool closed;
};

#endif //ISLAY_INTERTHREADMESSENGER_H
