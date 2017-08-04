import { Observable } from 'rxjs/Rx';
import { TimePoint } from './seer-protocol';
/**
 * Parse and provides access to an seer connection.
 * @export
 * @class Seer
 */
export class Seer {
    private readonly source: Observable<any>;
    /**
     * Creates an instance of Seer.
     * @param {Observable<any>} source it should consume and parse.
     * @memberof Seer
     */
    constructor(source: Observable<any>) {
        this.source = source;
    }

    public timePoints(): Observable<TimePoint> {

    }

    private parse
}