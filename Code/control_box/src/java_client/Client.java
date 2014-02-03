import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;

public class Client
{

    /** The channel to the control box. */
    private SocketChannel channel;
    private ByteBuffer buffer = ByteBuffer.allocate(1024);

    private static final String SET_PAN_TILT_POSITIONS = "SPTP";
    private static final String GET_RELAY_SETTINGS = "GRLS";
    private static final String SET_RELAY_SETTINGS = "SRLS";
    private static final String GET_RELAY_N = "GRLN";
    private static final String SET_RELAY_N = "SRLN";
    private static final String RESET_RELAY_N = "RRLN";
    private static final String GET_INCLINOMETER = "GINC";
    private static final String OK = "OK";
    private static final String EXIT = "EXIT";
    private static final String SHUTDOWN = "EEEE";
    
    private static final String BOX_HOSTNAME = "hostname";
    private static final String BOX_PORT = "port";

        public static void main(String[] args) {
	int temp;
        float values[] = new float[2];
                Client c = new Client();
	  try {
		c.connect();
		values = c.setPanTilt(2.7F,4.3F);
                System.out.println("value 0 = " + values[0] + " value 1 = " + values[1]);
                int s = c.getRelays();
                System.out.println("current relay settings = " + s);
                if (c.setRelays(109)) System.out.println("relays set o.k.");
                        else System.out.println("relays NOT set");
		temp = 4;
                if (c.setRelayN(temp)) System.out.println("relay " + temp + " set o.k.");
                        else System.out.println("relay " + temp + " NOT set");
                s = c.getRelays();
                System.out.println("current relay settings = " + s);
		temp = 3;
                s = c.getRelayN(temp);
                System.out.println("current relay " + temp + " settings = " + s);
                if (c.resetRelayN(5)) System.out.println("relay N reset o.k.");
                        else System.out.println("relay N NOT set");
		values = c.getInclinometer();
                System.out.println("side-side= " + values[0] + " fore-aft= " + values[1]);
		c.disconnect();
		// c.shutdown();
          } catch ( Exception e ) {
             System.err.println("threw exception " + e);
          } ;
        }

    /**
     * Constructs the client.
     */
    public Client()
    {

    }

    /**
     * Connects to the control box
     * @throws IOException
     */
    public void connect() throws IOException
    {
        InetSocketAddress address = new InetSocketAddress(
                System.getProperty(BOX_HOSTNAME, "127.0.0.1"), 
                Integer.getInteger(BOX_PORT, 4675));
        channel = SocketChannel.open(address);
    }

    /**
     * Disconnects from control box
     * @throws IOException
     */
    public void disconnect() throws IOException
    {
        if (null != channel)
        {
            sendText(EXIT);
            channel.close();
        }
    }

    /**
     * Shutdown the whole control box
     * @throws IOException
     */
    public void shutdown() throws IOException
    {
        if (null != channel)
        {
            sendText(SHUTDOWN);
            channel.close();
        }
    }

    /**
     * Set all the relay on the control box
     * @return The relay settings
     * @throws IOException
     */
    public boolean setRelays(int value) throws IOException
    {
        sendText(SET_RELAY_SETTINGS);

        writeInt(value);

        String ack = readText(2);
        if (!ack.equals(OK))
        {
            System.err.println("Unable to set relays box_control returned " + ack);
            return false;
        }
        return true;

    }

    /**
     * Set a single relay on the control box
     * @return true/false
     * @throws IOException
     */
    public boolean setRelayN(int relay) throws IOException
    {
        sendText(SET_RELAY_N);

        writeInt(relay);

        String ack = readText(2);
        if (!ack.equals(OK))
        {
            System.err.println("Unable to set relay " + relay + ", box_control returned " + ack);
            return false;
        }
        return true;

    }

    /**
    /**
     * Reset a single relay on the control box
     * @return true/false
     * @throws IOException
     */
    public boolean resetRelayN(int relay) throws IOException
    {
        sendText(RESET_RELAY_N);

        writeInt(relay);

        String ack = readText(2);
        if (!ack.equals(OK))
        {
            System.err.println("Unable to set relay " + relay + ", box_control returned " + ack);
            return false;
        }
        return true;

    }

    /**
     * Get the current relay settings from the control box
     * @return The relay settings
     * @throws IOException
     */
    public int getRelays() throws IOException
    {
        sendText(GET_RELAY_SETTINGS);

        int settings = readInt();
        return settings;
    }

    /**
     * Get the current relay settings from the control box
     * @return The relay settings
     * @throws IOException
     */
    public int getRelayN(int relay) throws IOException
    {
        sendText(GET_RELAY_N);

        writeInt(relay);
        int settings = readInt();
        return settings;
    }

    /**
     * Sets the pan and the tilt values of the PanCam.
     * @param pan The pan between 0 and 360, with 0 looking backwards over the
     * left of the rover.
     * @param tilt The tilt between 0 and 180, with 0 looking down.
     * @return The actual pan and tilt values respectively in the array.
     * @throws IOException
     */
    public float[] setPanTilt(float pan, float tilt) throws IOException
    {
        sendText(SET_PAN_TILT_POSITIONS);
        writeFloat(pan);
        writeFloat(tilt);

        float[] retVal = new float[2];
        retVal[0] = readFloat();
        retVal[1] = readFloat();
        sendText(OK);

        return retVal;
    }

    /**
     * Get the current values from the inclinometer
     * @return The actual side-side and fore-aft values respectively in the array.
     * @throws IOException
     */
    public float[] getInclinometer() throws IOException
    {
        sendText(GET_INCLINOMETER);

        float[] retVal = new float[2];
        retVal[0] = readFloat();
        retVal[1] = readFloat();
        return retVal;
    }

    /**
     * Sends some text to the agent interface.
     * @param text The text to send.
     * @throws IOException
     */
    private void sendText(String text) throws IOException
    {
        ByteBuffer buffer = Charset.defaultCharset().encode(text);
        channel.write(buffer);
    }

    /**
     * Sends some text to the agent interface.
     * @param text The text to send.
     * @throws IOException
     */
    private void sendNullTerminatedString(String text) throws IOException
    {
        buffer.clear();
        buffer.putInt(text.length());
        buffer.flip();
        channel.write(buffer);
        sendText(text);
    }

    /**
     * Reads a given number of characters from the channel.
     * @param length The number of characters to read.
     * @return The resultant string.
     * @throws IOException
     */
    private String readText(int length) throws IOException
    {
        buffer.clear();
        channel.read(buffer);
        buffer.flip();
        return Charset.defaultCharset().decode(buffer).subSequence(0, length).toString();
    }

    /**
     * Reads a int from the channel.
     * @return The resultant int.
     * @throws IOException
     */
    private int readInt() throws IOException
    {
        buffer.clear();
        buffer.limit(4); // limit the buffer to be the size of an int
        channel.read(buffer);
        buffer.flip();
        return buffer.getInt();
    }

    /**
     * Reads a float from the channel.
     * @return The resultant float.
     * @throws IOException
     */
    private float readFloat() throws IOException
    {
        buffer.clear();
        buffer.limit(4); // limit the buffer to be the size of a float
        channel.read(buffer);
        buffer.flip();
        return buffer.getFloat();
    }

    /**
     * Writes a float into the buffer and writes it to the channel.
     * @param value The float.
     * @throws IOException
     */
    private void writeFloat(float value) throws IOException
    {
        buffer.clear();
        buffer.putFloat(value);
        buffer.flip();
        channel.write(buffer);
    }

    /**
     * Writes a int into the buffer and writes it to the channel.
     * @param value The int.
     * @throws IOException
     */
    private void writeInt(int value) throws IOException
    {
        buffer.clear();
        buffer.putInt(value);
        buffer.flip();
        channel.write(buffer);
    }
}
