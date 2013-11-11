/*
 * Copyright 2008-2013 www.pretty-tools.com
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

package com.pretty_tools.dde.client;

import com.pretty_tools.dde.ClipboardFormat;
import com.pretty_tools.dde.DDEException;

import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * This class represents a Dynamic Data Exchange (DDE) Conversation.
 * DDE always occurs between a client application and a server application.
 * The DDE client application initiates the exchange by establishing
 * a conversation with the server to send transactions to the server.
 * A transaction is a request for data or services. The DDE server
 * application responds to transactions by providing data or services
 * to the client. (More information can be found in MSDN).
 * <p>
 * This implementation supports only synchronous transaction.
 * If synchronous transaction takes longer to execute then timeout
 * specified by {@link #setTimeout(int)} then method throws DDEMLException.
 * </p>
 *
 * <p>
 * This class supports only client following transactions:
 * <ul>
 * <li>XTYP_ADVSTART transaction to establish an advise loop with a server</li>
 * <li>XTYP_ADVSTOP transaction to end an advise loop with a server</li>
 * <li>XTYP_EXECUTE transaction to send a command string to the server</li>
 * <li>XTYP_POKE transaction to send unsolicited data to the server</li>
 * <li>XTYP_REQUEST transaction to request data from a server.</li>
 * </ul>
 * </p>
 * <p>
 * Advice transactions add items for DDE server to notify about their changes.
 * You get these notifications via {@link DDEClientEventListener#onItemChanged(String, String, String)}.
 * </p>
 * <p>
 * This class is thread-safe. All calls to WinAPI are dispatched to DDE working thread.
 * DDE asynchronous events are dispatched through {@link ExecutorService} to {@link DDEClientEventListener}s.
 * </p>
 * @author Alexander Kozlov (alex@pretty-tools.com)
 */
public class DDEClientConversation
{
    private static final Logger logger = Logger.getLogger(DDEClientConversation.class.getName());

    private static final int DEFAULT_TIMEOUT = 2000;

    /** DDE Client Events Listener. */
    private volatile DDEClientEventListener eventListener;
    /** Executor service to fire notifications in different thread. */
    private ExecutorService executorService;
    /** Service name of the server application with which a conversation is established. */
    private String service;
    /** Name of the topic on which a conversation is to be established. */
    private String topic;
    /** Maximum amount of time, in milliseconds, that the client will wait for a response from the server application in a synchronous transaction. */
    private int timeout = DEFAULT_TIMEOUT;
    /** Standard clipboard format that will be used for textual data. It can be CF_TEXT or CF_UNICODETEXT. */
    private int textFormat = ClipboardFormat.CF_TEXT.getNativeCode();

    public DDEClientConversation()
    {
    }

    /**
     * Establish conversation with specified service and topic. Service must support CF_TEXT format.
     *
     * @param service  service name
     * @param topic    topic name
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void connect(String service, String topic) throws DDEException
    {
        if (nativeDDEClientConversation != 0)
            throw new DDEException("Already connected.");

        executorService = Executors.newSingleThreadExecutor();

        nativeConnect(service, topic);

        this.service = service;
        this.topic = topic;
    }

    /**
     * Terminate current conversation.
     *
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void disconnect() throws DDEException
    {
        if (executorService != null)
        {
            executorService.shutdownNow();
            executorService = null;
        }

        if (nativeDDEClientConversation != 0) nativeDisconnect();

        this.service = null;
        this.topic = null;
    }

    /**
     * Send a command string to the server.
     *
     * @param command command to execute
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void execute(String command) throws DDEException
    {
        checkConversation();

        nativeExecute(command);
    }

    /**
     * Send a command to the server.
     *
     * @param command command to execute
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void execute(byte[] command) throws DDEException
    {
        checkConversation();

        nativeExecute(command);
    }

    /**
     * Send unsolicited data to the server.
     *
     * @param item item name
     * @param data data to send
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void poke(String item, String data) throws DDEException
    {
        checkConversation();

        nativePoke(item, data);
    }

    /**
     * Send unsolicited data to the server.
     *
     * @param item item name
     * @param data data to send
     * @param clipboardFormat Specifies the standard clipboard format of the data.
     * @throws com.pretty_tools.dde.DDEException
     */
    public void poke(String item, byte[] data, ClipboardFormat clipboardFormat) throws DDEException
    {
        poke(item, data, clipboardFormat.getNativeCode());
    }

    /**
     * Send unsolicited data to the server.
     *
     * @param item item name
     * @param data data to send
     * @param wFmt Specifies the standard clipboard format of the data.
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void poke(String item, byte[] data, int wFmt) throws DDEException
    {
        checkConversation();

        nativePoke(item, data, wFmt);
    }

    /**
     * Request service for specified item.
     *
     * @param item item name
     * @return Item data.
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized String request(String item) throws DDEException
    {
        checkConversation();

        return nativeRequest(item);
    }

    /**
     * Establish an advise loop with a server.
     *
     * @param item item name
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void startAdvice(String item) throws DDEException
    {
        checkConversation();

        nativeStartAdvice(item);
    }

    /**
     * End an advise loop with a server.
     *
     * @param item item name
     * @throws com.pretty_tools.dde.DDEException
     */
    public synchronized void stopAdvice(String item) throws DDEException
    {
        checkConversation();

        nativeStopAdvice(item);
    }

    /**
     * @return Name of the topic on which a conversation is to be established.
     */
    public String getService()
    {
        return service;
    }

    /**
     * @return Service name of the server application with which a conversation is established.
     */
    public String getTopic()
    {
        return topic;
    }

    /**
     * @return Timeout in milliseconds.
     */
    public int getTimeout()
    {
        return timeout;
    }

    /**
     * Specifies the maximum amount of time, in milliseconds,
     * that the client will wait for a response from the server
     * application in a synchronous transaction.<br>
     * Should be called before {@link #connect(String, String)}.
     *
     * @param timeout timeout in milliseconds.
     * @throws IllegalArgumentException if timeout is less or equal to zero.
     * @throws IllegalStateException if connect was established.
     */
    public void setTimeout(int timeout)
    {
        if (timeout <= 0)
            throw new IllegalArgumentException("Timeout should be bigger than zero");

        if (nativeDDEClientConversation != 0)
            throw new IllegalStateException("You cannot change timeout after connect was established");

        this.timeout = timeout;
    }

    /**
     * @return clipboard format for text.
     */
    public ClipboardFormat getTextFormat()
    {
        return ClipboardFormat.valueOf(this.textFormat);
    }

    /**
     * Specifies clipboard format text during DDE conversation.
     *
     * @param textFormat clipboard format for text. It cab be CF_TEXT or CF_UNICODETEXT.
     * @throws IllegalArgumentException if format neither CF_TEXT nor CF_UNICODETEXT.
     * @throws IllegalStateException if connect was established.
     */
    public void setTextFormat(ClipboardFormat textFormat)
    {
        if (textFormat != ClipboardFormat.CF_TEXT && textFormat != ClipboardFormat.CF_UNICODETEXT)
            throw new IllegalArgumentException("Format should be CF_TEXT or CF_UNICODETEXT");

        if (nativeDDEClientConversation != 0)
            throw new IllegalStateException("You cannot change timeout after connect was established");

        this.textFormat = textFormat.getNativeCode();
    }

    /**
     * @return Event listener.
     */
    public DDEClientEventListener getEventListener()
    {
        return eventListener;
    }

    /**
     * Set event listener.
     *
     * @param eventListener event listener
     */
    public void setEventListener(DDEClientEventListener eventListener)
    {
        this.eventListener = eventListener;
    }

    /**
     * Checks that conversation is established and throws exception otherwise.
     *
     * @throws com.pretty_tools.dde.DDEException if it is not connected
     */
    private void checkConversation() throws DDEException
    {
        if (nativeDDEClientConversation == 0)
            throw new DDEException("Not connected to server.");
    }

    /**
     * Fire OnDisconnect event.
     * Called from c++ code.
     */
    private void fireEventOnDisconnect()
    {
        try
        {
            final long nativeDDEClientWhenDisconnectHappens = nativeDDEClientConversation;

            executorService.submit(new Runnable(){
                public void run()
                {
                    try
                    {
                        final DDEClientEventListener listener = getEventListener();

                        if (listener != null) listener.onDisconnect();
                    }
                    catch (Throwable t)
                    {
                        logger.log(Level.SEVERE, t.toString(), t);
                    }
                    // disconnect and free resources
                    try
                    {
                        synchronized (DDEClientConversation.this)
                        {
                            if (nativeDDEClientWhenDisconnectHappens == nativeDDEClientConversation && nativeDDEClientConversation != 0)
                                disconnect();
                        }
                    }
                    catch (Throwable t)
                    {
                        logger.log(Level.SEVERE, t.toString(), t);
                    }
                }
            });
        }
        catch (Throwable thr)
        {
            logger.log(Level.SEVERE, thr.toString(), thr);
        }
    }

    /**
     * Fire OnAdvData event.
     * Called from c++ code.
     */
    private void fireEventOnAdvData(final String topic, final String item, final String data)
    {
        if (getEventListener() == null)
            return;

        try
        {
            executorService.submit(new Runnable(){
                public void run()
                {
                    try
                    {
                        final DDEClientEventListener listener = getEventListener();

                        if (listener != null) listener.onItemChanged(topic, item, data);
                    }
                    catch (Exception e)
                    {
                        logger.log(Level.SEVERE, e.toString(), e);
                    }
                }
            });
        }
        catch (Throwable thr)
        {
            logger.log(Level.SEVERE, thr.toString(), thr);
        }
    }

    // Pointer to c++ DEEClient implementation. Used from c++ code.
    private volatile long nativeDDEClientConversation;

    private synchronized native void nativeConnect(String server, String topic) throws UnsatisfiedLinkError, DDEException;
    private synchronized native void nativeDisconnect() throws UnsatisfiedLinkError, DDEException;

    private synchronized native void nativeExecute(String command) throws UnsatisfiedLinkError, DDEException;
    private synchronized native void nativeExecute(byte[] command) throws UnsatisfiedLinkError, DDEException;
    private synchronized native void nativePoke(String item, String data) throws UnsatisfiedLinkError, DDEException;
    private synchronized native void nativePoke(String item, byte[] data, int wFmt) throws UnsatisfiedLinkError, DDEException;
    private synchronized native String nativeRequest(String item) throws UnsatisfiedLinkError, DDEException;

    private synchronized native void nativeStartAdvice(String item) throws UnsatisfiedLinkError, DDEException;
    private synchronized native void nativeStopAdvice(String item) throws UnsatisfiedLinkError, DDEException;

    static// Loads the library, if available.
    {
        if ("64".equals(System.getProperty("sun.arch.data.model")))
            System.loadLibrary("JavaDDEx64");
        else
            System.loadLibrary("JavaDDE");
    }
}
