import com.pretty_tools.dde.client.DDEClientConversation;
import com.pretty_tools.dde.client.DDEClientException;
import com.pretty_tools.dde.client.DDEMLException;

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

conversation.setTimeout(3000);
conversation.connect("Excel", "Sheet1");
try
{
// Requesting A1 value
System.out.println("A1 value: " + conversation.request("R1C1"));
// Changing cell A1 value to "We did it!"
conversation.poke("R1C1", "We did it!");
conversation.poke("R2C2", "We did it again!".getBytes());
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
catch (DDEClientException e)
{
System.out.println("DDEClientException: " + e.getMessage());
}
catch (Exception e)
{
System.out.println("Exception: " + e);
}
}
}
