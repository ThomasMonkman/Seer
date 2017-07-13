import { ConnectionMeta } from 'app/classes/connnection-meta';

export class Tab {
    public readonly connectionMeta: ConnectionMeta = null;
    public readonly newTab: boolean = false;
    constructor(connectionMeta: ConnectionMeta = null) {
        if (connectionMeta === null) {
            this.newTab = true;
        }
        this.connectionMeta = connectionMeta;
    }
}
