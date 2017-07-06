export class ConnectionMeta {
    public readonly name: string;
    public readonly address: string;
    constructor(name: string, address: string) {
        this.name = name;
        this.address = address.toLocaleLowerCase();
    }
}