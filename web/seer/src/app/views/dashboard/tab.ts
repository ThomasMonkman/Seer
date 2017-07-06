export class Tab {
    public title: string;
    public address: string = null;
    constructor(title: string, address: string = null) {
        this.title = title;
        this.address = address;
    }
}