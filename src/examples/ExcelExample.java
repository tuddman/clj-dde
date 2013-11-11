/*
 * Copyright 2009 www.pretty-tools.com. All rights reserved.
 */

import com.pretty_tools.dde.ClipboardFormat;
import com.pretty_tools.dde.DDEException;
import com.pretty_tools.dde.DDEMLException;
import com.pretty_tools.dde.client.DDEClientConversation;

/**
 * Excel Example.
 *
 * @author Alexander Kozlov
 */
public class ExcelExample
{
    public static void main(String[] args)
    {
        try
        {
            // DDE client
            final DDEClientConversation conversation = new DDEClientConversation();
            // We can use UNICODE format if server prefers it
            //conversation.setTextFormat(ClipboardFormat.CF_UNICODETEXT);

            conversation.setTimeout(3000);
            conversation.connect("Excel", "Sheet1");
            try
            {
                // Requesting A1 value
                System.out.println("A1 value: " + conversation.request("R1C1"));
                // Changing cell A1 value to "We did it!"
                conversation.poke("R1C1", "We did it!");
                conversation.poke("R2C2", "We did it again!".getBytes(), ClipboardFormat.CF_TEXT);
                // Sending "close()" command
                conversation.execute("[close()]");
                // or we can use byte array to send command
                //conversation.execute("[close()]\0".getBytes());
            }
            finally
            {
                conversation.disconnect();
            }
        }
        catch (DDEMLException e)
        {
            System.out.println("DDEMLException: 0x" + Integer.toHexString(e.getErrorCode()) + " " + e.getMessage());
        }
        catch (DDEException e)
        {
            System.out.println("DDEClientException: " + e.getMessage());
        }
        catch (Exception e)
        {
            System.out.println("Exception: " + e);
        }
    }
}