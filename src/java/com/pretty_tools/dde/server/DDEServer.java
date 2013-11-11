/*
 * Copyright 2013 www.pretty-tools.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.pretty_tools.dde.server;

import com.pretty_tools.dde.DDEException;

import java.util.logging.Logger;
import java.util.logging.Level;

/**
 * This class represents a Dynamic Data Exchange (DDE) Server.
 * DDE always occurs between a client application and a server application.
 * The DDE client application initiates the exchange by establishing
 * a conversation with the server to send transactions to the server.
 * A transaction is a request for data or services. The DDE server
 * application responds to transactions by providing data or services
 * to the client.
 * <p>
 *     A server can have many clients at the same time, and a client
 * can request data from multiple servers. An application can also be
 * both a client and a server. Either the client or the server can
 * terminate the conversation at any time.
 * (More information can be found in MSDN).
 * </p>
 * <p>
 * This class is thread-safe. DDE exchange is made in separate thread,
 * and all methods will be called from it.
 * This implementation supports only synchronous transaction,
 * and since there is single thread per server instance you should not delay methods for a long time.
 * </p>
 *
 * @author Alexander Kozlov (alex@pretty-tools.com)
 */
public abstract class DDEServer
{
    private static final Logger logger = Logger.getLogger(DDEServer.class.getName());

    /** Service name of the server application that will be handled by this class. */
    private final String service;

    protected DDEServer(String service)
    {
        this.service = service;
    }

    /**
     * Start DDE service.
     * This function causes the system to send XTYP_REGISTER transactions to other running
     * Dynamic Data Exchange Management Library (DDEML) client applications.
     *
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void start() throws DDEException
    {
        if (nativeDDEServer != 0)
            throw new DDEException("Already started.");

        nativeStart(service);
    }

    /**
     * Stop DDE service.
     * This function causes the system to send XTYP_UNREGISTER transactions to other running
     * Dynamic Data Exchange Management Library (DDEML) client applications.
     *
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void stop() throws DDEException
    {
        if (nativeDDEServer != 0) nativeStop();
    }

    /**
     * Notify clients that item was changed.
     *
     * @param topic topic name of updated item
     * @param item item name which was updated
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void notifyClients(String topic, String item) throws DDEException
    {
        checkState();

        nativeNotifyClients(topic, item);
    }

    /**
     * Override this method to filter topic names which server will handle.
     *
     * @param topic topic name to check
     * @return <code>true</code> if topic is supported or <code>false</code> otherwise.
     */
    protected boolean isTopicSupported(String topic)
    {
        return true;
    }

    /**
     * Override this method to filter item names to which client can advise.
     *
     * @param topic topic name to check
     * @param item item name to check
     * @param uFmt specifies the data format requested by the client
     * @return <code>true</code> if topic and item is supported for advise or <code>false</code> otherwise.
     */
    protected boolean isItemSupported(String topic, String item, int uFmt)
    {
        return isTopicSupported(topic);
    }

    /**
     * Override this method to process commands.
     *
     * @param command command to process.
     * @return <code>true</code> if command was processed or <code>false</code> to reject command (in that case native code will return DDE_FNOTPROCESSED).
     */
    protected boolean onExecute(String command)
    {
        return false;
    }

    /**
     * Override this method to process poke request.
     * This method will be called when textual data is sent.
     *
     * @param topic topic to process.
     * @param item item to update.
     * @param data textual data.
     * @return <code>true</code> if request was processed or <code>false</code> to reject request (in that case native code will return DDE_FNOTPROCESSED).
     */
    protected boolean onPoke(String topic, String item, String data)
    {
        return false;
    }

    /**
     * Override this method to process poke request.
     * This method will be called when non-textual data is sent.
     *
     * @param topic topic to process.
     * @param item item to update.
     * @param data data.
     * @param uFmt Specifies the clipboard format in which the server should submit data to the client.
     * @return <code>true</code> if request was processed or <code>false</code> to reject request (in that case native code will return DDE_FNOTPROCESSED).
     */
    protected boolean onPoke(String topic, String item, byte[] data, int uFmt)
    {
        return false;
    }

    /**
     * Override this method to process data request.
     * This method will be called when textual data is required.
     *
     * @param topic topic to process.
     * @param item item to return.
     * @return item text.
     */
    protected String onRequest(String topic, String item)
    {
        return null;
    }

    /**
     * Override this method to process data request.
     * This method will be called when non-textual data is required.
     *
     * @param topic topic to process.
     * @param item item to return.
     * @param uFmt Specifies the clipboard format in which the server should submit data to the client.
     * @return data bytes.
     */
    protected byte[] onRequest(String topic, String item, int uFmt)
    {
        return null;
    }

    /**
     * Checks that conversation is established and throws exception otherwise.
     *
     * @throws com.pretty_tools.dde.DDEException if it is not connected
     */
    private void checkState() throws DDEException
    {
        if (nativeDDEServer == 0)
            throw new DDEException("Server was not started.");
    }

    // Pointer to c++ DEEServer implementation. Used from c++ code.
    private volatile long nativeDDEServer;

    private synchronized native void nativeStart(String service) throws UnsatisfiedLinkError, DDEException;
    private synchronized native void nativeStop() throws UnsatisfiedLinkError, DDEException;
    private synchronized native void nativeNotifyClients(String topic, String item) throws UnsatisfiedLinkError, DDEException;

    static// Loads the library, if available.
    {
        if ("64".equals(System.getProperty("sun.arch.data.model")))
            System.loadLibrary("JavaDDEx64");
        else
            System.loadLibrary("JavaDDE");
    }
}
