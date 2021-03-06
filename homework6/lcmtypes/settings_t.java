/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package lcmtypes;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class settings_t implements lcm.lcm.LCMEncodable
{
    public int initial_runner_idx;
    public int delay_every;
    public double to_goal_magnitude;
    public int to_goal_power;
    public double avoid_obs_magnitude;
    public int avoid_obs_power;
    public double max_vel;
 
    public settings_t()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0xa5c17cd2bfaee412L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(lcmtypes.settings_t.class))
            return 0L;
 
        classes.add(lcmtypes.settings_t.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeInt(this.initial_runner_idx); 
 
        outs.writeInt(this.delay_every); 
 
        outs.writeDouble(this.to_goal_magnitude); 
 
        outs.writeInt(this.to_goal_power); 
 
        outs.writeDouble(this.avoid_obs_magnitude); 
 
        outs.writeInt(this.avoid_obs_power); 
 
        outs.writeDouble(this.max_vel); 
 
    }
 
    public settings_t(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public settings_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static lcmtypes.settings_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        lcmtypes.settings_t o = new lcmtypes.settings_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.initial_runner_idx = ins.readInt();
 
        this.delay_every = ins.readInt();
 
        this.to_goal_magnitude = ins.readDouble();
 
        this.to_goal_power = ins.readInt();
 
        this.avoid_obs_magnitude = ins.readDouble();
 
        this.avoid_obs_power = ins.readInt();
 
        this.max_vel = ins.readDouble();
 
    }
 
    public lcmtypes.settings_t copy()
    {
        lcmtypes.settings_t outobj = new lcmtypes.settings_t();
        outobj.initial_runner_idx = this.initial_runner_idx;
 
        outobj.delay_every = this.delay_every;
 
        outobj.to_goal_magnitude = this.to_goal_magnitude;
 
        outobj.to_goal_power = this.to_goal_power;
 
        outobj.avoid_obs_magnitude = this.avoid_obs_magnitude;
 
        outobj.avoid_obs_power = this.avoid_obs_power;
 
        outobj.max_vel = this.max_vel;
 
        return outobj;
    }
 
}

