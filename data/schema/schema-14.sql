ALTER TABLE %allsongstables ADD COLUMN fingerprint TEXT;

ALTER TABLE %allsongstables ADD COLUMN lastseen INTEGER NOT NULL DEFAULT 0;

UPDATE schema_version SET version=14;
