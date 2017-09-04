export class BasePoint {
    public readonly type: string;
    constructor(json: any) {
        this.type = json['#'];
    }
}
