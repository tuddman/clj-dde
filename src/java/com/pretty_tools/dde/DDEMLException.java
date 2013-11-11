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

package com.pretty_tools.dde;

/**
 * The Dynamic Data Exchange Management Library Exception.
 *
 * @author Alexander Kozlov
 */
public class DDEMLException extends DDEException
{
    /** A request for a synchronous advise transaction has timed out. */
    public static final int DMLERR_ADVACKTIMEOUT = 0x4000;
    /** The response to the transaction caused the DDE_FBUSY flag to be set. */
    public static final int DMLERR_BUSY = 0x4001;
    /** A request for a synchronous data transaction has timed out. */
    public static final int DMLERR_DATAACKTIMEOUT = 0x4002;
    /**
     * A DDEML function was called without first calling the DdeInitialize function,
     * or an invalid instance identifier was passed to a DDEML function.
     */
    public static final int DMLERR_DLL_NOT_INITIALIZED = 0x4003;
    /**
     * An application initialized as APPCLASS_MONITOR has attempted to perform a Dynamic Data Exchange (DDE) transaction,
     * or an application initialized as APPCMD_CLIENTONLY has attempted to perform server transactions.
     */
    public static final int DMLERR_DLL_USAGE = 0x4004;
    /** A request for a synchronous execute transaction has timed out. */
    public static final int DMLERR_EXECACKTIMEOUT = 0x4005;
    /**
     * A parameter failed to be validated by the DDEML. Some of the possible causes follow:<br>
     * <br>
     * The application used a data handle initialized with a different item name handle than was required by the transaction.<br>
     * <br>
     * The application used a data handle that was initialized with a different clipboard data format than was required by the transaction.<br>
     * <br>
     * The application used a client-side conversation handle with a server-side function or vice versa.<br>
     * <br>
     * The application used a freed data handle or string handle.<br>
     * <br>
     * More than one instance of the application used the same object.<br>
     */
    public static final int DMLERR_INVALIDPARAMETER = 0x4006;
    /**
     * A DDEML application has created a prolonged race condition (in which the server application outruns the client),
     * causing large amounts of memory to be consumed.
     */
    public static final int DMLERR_LOW_MEMORY = 0x4007;
    /** A memory allocation has failed. */
    public static final int DMLERR_MEMORY_ERROR = 0x4008;
    /** A transaction has failed. */
    public static final int DMLERR_NOTPROCESSED = 0x4009;
    /** A client's attempt to establish a conversation has failed. */
    public static final int DMLERR_NO_CONV_ESTABLISHED = 0x400a;
    /** A request for a synchronous poke transaction has timed out. */
    public static final int DMLERR_POKEACKTIMEOUT = 0x400b;
    /** An internal call to the PostMessage function has failed. */
    public static final int DMLERR_POSTMSG_FAILED = 0x400c;
    /**
     * An application instance with a synchronous transaction already in progress attempted to initiate another
     * synchronous transaction, or the DdeEnableCallback function was called from within a DDEML callback function.
     */
    public static final int DMLERR_REENTRANCY = 0x400d;
    /**
     * A server-side transaction was attempted on a conversation terminated by the client,
     * or the server terminated before completing a transaction.
     */
    public static final int DMLERR_SERVER_DIED = 0x400e;
    /** An internal error has occurred in the DDEML. */
    public static final int DMLERR_SYS_ERROR = 0x400f;
    /** A request to end an advise transaction has timed out. */
    public static final int DMLERR_UNADVACKTIMEOUT = 0x4010;
    /**
     * An invalid transaction identifier was passed to a DDEML function.
     * Once the application has returned from an XTYP_XACT_COMPLETE callback,
     * the transaction identifier for that callback function is no longer valid.
     */
    public static final int DMLERR_UNFOUND_QUEUE_ID = 0x4011;

    /** Dynamic Data Exchange Management Library error code. */
    private int errorCode;

    public DDEMLException(int errorCode, String message)
    {
        super(message);
        this.errorCode = errorCode;
    }

    /**
     * @return Dynamic Data Exchange Management Library error code.
     */
    public int getErrorCode()
    {
        return errorCode;
    }
}