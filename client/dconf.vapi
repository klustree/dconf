/* dconf.vapi generated by valac 0.17.1.35-814b, do not modify. */

namespace DConf {
	[CCode (cheader_filename = "dconf.h", cprefix="DCONF_READ_")]
	public enum ReadFlags {
		[CCode (cname="DCONF_READ_FLAGS_NONE")]
		NONE,
		DEFAULT_VALUE,
		USER_VALUE
	}

	[CCode (cheader_filename = "dconf.h")]
	public class Client : GLib.Object {
		public signal void changed (string prefix, [CCode (array_length = false, array_null_terminated = true)] string[] changes, string? tag);

		public Client ();
		public GLib.Variant? read (string key);
		public GLib.Variant? read_full (string key, ReadFlags flags, GLib.Queue<Changeset>? read_through);
		public string[] list (string dir);
		public string[] list_locks (string dir);
		public bool is_writable (string key);
		public void write_fast (string path, GLib.Variant? value) throws GLib.Error;
		public void write_sync (string path, GLib.Variant? value, out string tag = null, GLib.Cancellable? cancellable = null) throws GLib.Error;
		public void change_fast (Changeset changeset) throws GLib.Error;
		public void change_sync (Changeset changeset, out string tag = null, GLib.Cancellable? cancellable = null) throws GLib.Error;
		public void watch_fast (string path);
		public void unwatch_fast (string path);
		public void watch_sync (string path);
		public void unwatch_sync (string path);
	}

	[Compact]
	[CCode (ref_function = "dconf_changeset_ref", unref_function = "dconf_changeset_unref")]
	public class Changeset {
		public delegate bool Predicate (string path, GLib.Variant? value);
		public Changeset ();
		public Changeset.write (string path, GLib.Variant? value);
		public void set (string path, GLib.Variant? value);
		public bool get (string path, out GLib.Variant? value);
		public bool is_similar_to (Changeset other);
		public bool all (Predicate predicate);
		public GLib.Variant serialise ();
		public static Changeset deserialise (GLib.Variant serialised);
	}

	[CCode (cheader_filename = "dconf.h")]
	public static bool is_dir (string str, GLib.Error* error = null);
	[CCode (cheader_filename = "dconf.h")]
	public static bool is_key (string str, GLib.Error* error = null);
	[CCode (cheader_filename = "dconf.h")]
	public static bool is_path (string str, GLib.Error* error = null);
	[CCode (cheader_filename = "dconf.h")]
	public static bool is_rel_dir (string str, GLib.Error* error = null);
	[CCode (cheader_filename = "dconf.h")]
	public static bool is_rel_key (string str, GLib.Error* error = null);
	[CCode (cheader_filename = "dconf.h")]
	public static bool is_rel_path (string str, GLib.Error* error = null);
	[CCode (cheader_filename = "dconf.h", cname = "dconf_is_dir")]
	public static bool verify_dir (string str) throws GLib.Error;
	[CCode (cheader_filename = "dconf.h", cname = "dconf_is_key")]
	public static bool verify_key (string str) throws GLib.Error;
	[CCode (cheader_filename = "dconf.h", cname = "dconf_is_path")]
	public static bool verify_path (string str) throws GLib.Error;
	[CCode (cheader_filename = "dconf.h", cname = "dconf_is_rel_dir")]
	public static bool verify_rel_dir (string str) throws GLib.Error;
	[CCode (cheader_filename = "dconf.h", cname = "dconf_is_rel_key")]
	public static bool verify_rel_key (string str) throws GLib.Error;
	[CCode (cheader_filename = "dconf.h", cname = "dconf_is_rel_path")]
	public static bool verify_rel_path (string str) throws GLib.Error;
}
