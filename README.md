pg\_schema\_triggers 0.1
========================

This extension adds schema change events to the [Event Triggers](http://www.postgresql.org/docs/9.4/static/event-triggers.html) feature in
PostgreSQL 9.3 and higher.  By using the ProcessUtility hook, it transparently
adds new event types to the `CREATE EVENT TRIGGER` command.  These new events
include column add, drop, and alter as well as table create and rename.

Each event has access to relevant information, such as the table's oid and
the old and new names.


New Events
----------

This extension adds support for several additional events that may be used with
the `CREATE EVENT TRIGGER` command.

    Event Name            Description
    --------------------  ----------------------------------------------------
    relation_create       New relation (table, view, or index) created;  note
                          that at the point that this event fires, the table's
                          constraints and column defaults have NOT yet been
                          created.  [This corresponds to the OAT_POST_CREATE
                          hook.]

                          From the event trigger function, calling the
                          get_relation_create_eventinfo() function will return
                          a RELATION_CREATE_EVENTINFO record: 

                              relation      REGCLASS
                              new           PG_CATALOG.PG_CLASS


    relation_alter        An existing relation has been altered.  [This event
                          corresponds to the OAT_POST_ALTER hook.]

                          From the event trigger function, calling the
                          get_relation_alter_eventinfo() function will return
                          a RELATION_ALTER_EVENTINFO record: 

                              relation      REGCLASS
                              old           PG_CATALOG.PG_CLASS
                              new           PG_CATALOG.PG_CLASS


    relation_drop         An existing relation has been dropped.  [This event
                          corresponds to the OAT_DROP hook.]

                          From the event trigger function, calling the
                          get_relation_drop_eventinfo() function will return
                          a RELATION_DROP_EVENTINFO record: 

                              old_relation_oid  REGCLASS
                              old               PG_CATALOG.PG_CLASS


    column_add            A new column has been added to a relation.  Note,
                          this will only be called for columns added *after*
                          a relation is first created.

                          From the event trigger function, calling the
                          get_column_add_eventinfo() function will return
                          a COLUMN_ADD_EVENTINFO record: 

                              relation      REGCLASS
                              attnum        INT16
                              new           PG_CATALOG.PG_ATTRIBUTE


    column_alter          An existing column has been altered.  [This event
                          corresponds to the OAT_POST_ALTER hook.]

                          From the event trigger function, calling the
                          get_column_alter_eventinfo() function will return
                          a COLUMN_ALTER_EVENTINFO record: 

                              relation      REGCLASS
                              attnum        INT16
                              old           PG_CATALOG.PG_ATTRIBUTE
                              new           PG_CATALOG.PG_ATTRIBUTE


    column_drop           An existing column has been dropped.

                          From the event trigger function, calling the
                          get_column_drop_eventinfo() function will return
                          a COLUMN_DROP_EVENTINFO record: 

                              relation      REGCLASS
                              attnum        INT16
                              old           PG_CATALOG.PG_ATTRIBUTE


    trigger_create        A new trigger has been created.

                          From the event trigger function, calling the
                          get_trigger_create_eventinfo() function will return
                          a TRIGGER_CREATE_EVENTINFO record: 

                              trigoid       OID
                              is_internal   BOOLEAN
                              new           PG_CATALOG.PG_TRIGGER


    trigger_drop          An existing trigger has been dropped.

                          From the event trigger function, calling the
                          get_trigger_drop_eventinfo() function will return
                          a TRIGGER_DROP_EVENTINFO record: 

                              trigoid       OID
                              old           PG_CATALOG.PG_TRIGGER


Examples
--------
This example issues a NOTICE whenever a table is created with a name that
begins with `test_`.

    postgres=# CREATE OR REPLACE FUNCTION on_relation_create()
               RETURNS event_trigger
               LANGUAGE plpgsql
               AS $$
                 DECLARE
                   event_info schema_triggers.RELATION_CREATE_EVENTINFO;
                 BEGIN
                   RAISE NOTICE 'on_relation_create()';
                   event_info := schema_triggers.get_relation_create_eventinfo();
                   IF NOT event_info.relation::text LIKE 'test_%' THEN RETURN; END IF;
                   RAISE NOTICE 'Relation (%) created in namespace (oid=%).',
                     event_info.relation,
                     (event_info.new).relnamespace;
                   RAISE NOTICE 'new pg_class row: %', event_info.new;
                 END;
               $$;
    CREATE FUNCTION
    postgres=# CREATE EVENT TRIGGER test_relations ON relation_create
               EXECUTE PROCEDURE on_relation_create();
    CREATE EVENT TRIGGER
    postgres=# create table foobar();
    CREATE TABLE
    postgres=# create table test_foobar();
    NOTICE:  Relation (test_foobar) created in namespace oid:(2200).
    NOTICE:  new pg_class row: (test_foobar,2200,49224,0,10,0,49222,0,0,0,0,0,0,f,f,p,r,0,0,f,f,f,f,f,t,924,1,,)
    CREATE TABLE


Build/install
-------------

This extension requires PostgreSQL 9.3 and higher, with the development packages and utilities (in particular, `pg_config`) installed.

To build, install, and test:

    $ make
    $ make install
    $ make installcheck

To enable this extension for a database, ensure that the extension has been
installed (`make install`) and then use the `CREATE EXTENSION` mechanism:

    CREATE EXTENSION schema_triggers;

In order for the extension to work (that is, for `CREATE EVENT TRIGGER` to
recognize the new events and for the event triggers to be fired upon those
events happening) the `pg_schema_triggers.so` library must be loaded.  Add
a line to `postgresql.conf`:

    shared_preload_libraries = 'schema_triggers.so'

Alternately, the new events can be enabled during a single session with the
`LOAD` command.  This is unlikely to be useful except during testing:

    LOAD 'schema_triggers.so';


Authors and Credits
-------------------

Andrew Tipton       andrew@kiwidrew.com

Developed by malloc() labs limited (www.malloclabs.com) for CartoDB.


License
-------
This PostgreSQL extension is free software;  you may use it under the same terms
as [PostgreSQL](http://postgresql.org) itself.  (See the `LICENSE` file for full licensing information.)
